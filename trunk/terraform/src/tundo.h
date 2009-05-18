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
 */


#include "tterrain.h"

typedef struct TUndoState TUndoState;
struct TUndoState
{
  gchar      *name;
  gint        serial;
  FILE       *heightfield;
  gint        width, height;
  gfloat      sealevel;
  TUndoState *down, *right;
};

void t_undo_state_revert (TUndoState *state,
                          TTerrain   *terrain);

typedef struct TUndo TUndo;
struct TUndo
{
  TUndoState *root;
  TUndoState *current;
  gint        node_count;
  gint        max_nodes;
  gint        serial;
};

TUndo      *t_undo_new          (gint        max_nodes);
void        t_undo_free         (TUndo      *undo);
TUndoState *t_undo_add_state    (TUndo      *undo,
                                 TTerrain   *terrain,
                                 gchar      *name);
void        t_undo_revert_state (TUndo      *undo,
                                 TUndoState *undo_state,
                                 TTerrain   *terrain);
GtkWidget  *t_undo_unpack_tree  (TUndo      *undo,
                                 GtkTree    *tree);
