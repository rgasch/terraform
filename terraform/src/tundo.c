/* Terraform - (C) 1997-2002 Robert Gasch (r.gasch@chello.nl)
 *  - http://terraform.sourceforge.net
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Code in this file is released under the following copyright:
 * Copyright (c) 2000-2002 David A. Bartold
 */


#include <stdio.h>
#include <gnome.h>
#include "tundo.h"

TUndo *
t_undo_new (gint max_nodes)
{
  TUndo *undo;

  undo = g_new0 (TUndo, 1);
  undo->max_nodes = MAX (max_nodes, 2);

  return undo;
}

static TUndoState *
t_undo_state_new (gchar      *name,
                  gint        serial,
                  TTerrain   *terrain)
{
  TUndoState *state;

  state = g_new0 (TUndoState, 1);
  state->name = g_strdup (name);
  state->serial = serial;
  state->width = terrain->width;
  state->height = terrain->height;
  state->sealevel = terrain->sealevel;
  state->heightfield = tmpfile ();
  fwrite (terrain->heightfield,
          sizeof (gfloat) * terrain->width * terrain->height, 1,
          state->heightfield);

  return state;
}

void
t_undo_state_revert (TUndoState *state,
                     TTerrain   *terrain)
{
  t_terrain_set_size (terrain, state->width, state->height);

  terrain->width = state->width;
  terrain->height = state->height;
  terrain->sealevel = state->sealevel;

  rewind (state->heightfield);
  fread (terrain->heightfield,
         sizeof (gfloat) * terrain->width * terrain->height, 1,
         state->heightfield);
}

static void
t_undo_state_free_1 (TUndoState *state)
{
  g_free (state->name);
  fclose (state->heightfield);
  g_free (state);
}

static void
t_undo_state_free (TUndoState *state)
{
  if (state != NULL)
    {
      t_undo_state_free (state->down);
      t_undo_state_free (state->right);
      t_undo_state_free_1 (state);
    }
}

void
t_undo_free (TUndo *undo)
{
  t_undo_state_free (undo->root);

  g_free (undo);
}

static gboolean
t_undo_state_in_subtree (TUndoState *subtree,
                         TUndoState *node)
{
  if (subtree != NULL)
    {
      if (subtree == node)
        return TRUE;

      return t_undo_state_in_subtree (subtree->right, node) ||
             t_undo_state_in_subtree (subtree->down, node);
    }

  return FALSE;
}

/*
 * Better algo: find node (and its parent) with oldest serial number which
 * isn't the undo->current.  Delete that node using some rules.
 *
 * This can be improved speedwise since we know:
 *   Property 1: A node is always older than its children
 *   Property 2: A node is always older than the rest of its siblings
 *
 * What we don't know: which is older ->right or ->down.
 *
 * Basically, we have at least a heap.  Perhaps something more like
 * heaps within a heap.
 */

static void
t_undo_trim (TUndo *undo)
{
  TUndoState *oldest, **parent_ptr;
  TUndoState *siblings, *children;
  TUndoState *state;

  while (undo->node_count >= undo->max_nodes)
    {
      /* Determine oldest node (not including the current one) */
      if (undo->root != undo->current)
        {
          parent_ptr = &undo->root;
        }
      else
        {
          if (undo->root->right == NULL)
            parent_ptr = &undo->root->down;
          else if (undo->root->down == NULL)
            parent_ptr = &undo->root->right;
          else
            parent_ptr = (undo->root->down->serial <
                          undo->root->right->serial) ?
                         &undo->root->down : &undo->root->right;
        }
      oldest = *parent_ptr;

      if (oldest->right != NULL)
        {
          /* If oldest->right != NULL, replace node with node->right */

          /*
           * Update strings of deleted node's children if it has
           * siblings and isn't the root node to prevent user confusion.
           *
           * Otherwise there will be a gap in the tree.
           */

          if (parent_ptr != &undo->root && oldest->down != NULL)
            {
              state = oldest->right;
              while (state != NULL)
                {
                  gchar *name;

                  name = g_strdup_printf ("%s + %s",
                                          oldest->name, state->name);

                  g_free (state->name);
                  state->name = name;

                  state = state->down;
                }
            }

          /*
           * Combine oldest's siblings with its modified children.
           *
           * We must merge the two lists (i.e. the list of siblings
           * and list of children) by serial number.
           *
           * We do this to keep Properties 1 and 2 which make it
           * trivial to find and remove the oldest node in the tree.
           */

          siblings = oldest->down;
          children = oldest->right;

          while (siblings != NULL && children != NULL)
            {
              if (siblings->serial < children->serial)
                {
                  *parent_ptr = siblings;
                  parent_ptr = &siblings->down;
                  siblings = *parent_ptr;
                }
              else
                {
                  *parent_ptr = children;
                  parent_ptr = &children->down;
                  children = *parent_ptr;
                }
            }

          while (siblings != NULL)
            {
              *parent_ptr = siblings;
              parent_ptr = &siblings->down;
              siblings = *parent_ptr;
            }

          while (children != NULL)
            {
              *parent_ptr = children;
              parent_ptr = &children->down;
              children = *parent_ptr;
            }

          *parent_ptr = NULL;
          undo->node_count--;
          t_undo_state_free_1 (oldest);
        }
      else /* oldest->right == NULL */
        {
          /* Update pointer from parent to new child */
          *parent_ptr = oldest->down;

          undo->node_count--;
          t_undo_state_free_1 (oldest);
        }
    }
}


TUndoState *
t_undo_add_state (TUndo      *undo,
                  TTerrain   *terrain,
                  gchar      *name)
{
  TUndoState *state;

  t_undo_trim (undo);
  state = t_undo_state_new (name, undo->serial++, terrain);
  undo->node_count++;

  if (undo->current == NULL)
    {
      undo->current = undo->root = state;
    }
  else if (undo->current->right != NULL)
    {
      undo->current = undo->current->right;
      while (undo->current->down != NULL)
        undo->current = undo->current->down;

      undo->current->down = state;
      undo->current = state;
    }
  else
    {
      undo->current->right = state;
      undo->current = state;
    }

  return state;
}

void
t_undo_revert_state (TUndo      *undo,
                     TUndoState *undo_state,
                     TTerrain   *terrain)
{
  t_undo_state_revert (undo_state, terrain);
  undo->current = undo_state;
}

static GtkWidget *
t_undo_unpack_subtree (TUndo      *undo,
                       TUndoState *state,
                       GtkTree    *tree)
{
  GtkWidget *selected;

  selected = NULL;
  while (state != NULL)
    {
      GtkWidget *item;

      item = gtk_tree_item_new_with_label (state->name);
      gtk_object_set_data (GTK_OBJECT (item), "data_state", state);
      gtk_widget_show (item);
      gtk_tree_append (tree, item);

      if (undo->current == state)
        selected = item;

      if (state->right != NULL)
        {
          GtkWidget *subtree;
          GtkWidget *temp;

          subtree = gtk_tree_new ();
          gtk_widget_show (subtree);
          gtk_tree_item_set_subtree (GTK_TREE_ITEM (item), subtree);
          gtk_tree_item_expand (GTK_TREE_ITEM (item));
          temp = t_undo_unpack_subtree (undo, state->right, GTK_TREE (subtree));

          if (temp != NULL)
            selected = temp;
        }

      state = state->down;
    }

  return selected;
}

GtkWidget *
t_undo_unpack_tree (TUndo      *undo,
                    GtkTree    *tree)
{
  GtkWidget *tree_root;
  GtkWidget *item;

  item = gtk_tree_item_new_with_label (_("Available States"));
  gtk_widget_show (item);
  gtk_tree_append (tree, item);

  tree_root = gtk_tree_new ();
  gtk_widget_show (tree_root);
  gtk_tree_item_set_subtree (GTK_TREE_ITEM (item), tree_root);
  gtk_tree_item_expand (GTK_TREE_ITEM (item));

  return t_undo_unpack_subtree (undo, undo->root, GTK_TREE (tree_root));
}
