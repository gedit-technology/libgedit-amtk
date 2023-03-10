/* SPDX-FileCopyrightText: 2017-2022 - Sébastien Wilmet <swilmet@gnome.org>
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "amtk-utils.h"
#include <string.h>
#include "amtk-action-info.h"
#include "amtk-action-info-central-store.h"

/**
 * SECTION:amtk-utils
 * @title: AmtkUtils
 * @short_description: Utility functions
 *
 * Utility functions.
 */

static gchar *
get_home_dir_without_trailing_slash (const gchar *home_dir)
{
	gchar *utf8_home_dir;
	gsize length;

	if (home_dir == NULL)
	{
		return NULL;
	}

	utf8_home_dir = g_filename_to_utf8 (home_dir, -1, NULL, NULL, NULL);
	if (utf8_home_dir == NULL)
	{
		return NULL;
	}

	length = strlen (utf8_home_dir);
	if (length == 0)
	{
		g_free (utf8_home_dir);
		return NULL;
	}

	if (utf8_home_dir[length - 1] == '/')
	{
		utf8_home_dir[length - 1] = '\0';
	}

	return utf8_home_dir;
}

/* Like tepl_utils_replace_home_dir_with_tilde() but with an additional home_dir
 * parameter, for unit tests.
 */
static gchar *
_tepl_utils_replace_home_dir_with_tilde_with_param (const gchar *filename,
						    const gchar *home_dir)
{
	gchar *home_dir_without_trailing_slash;
	gchar *home_dir_with_trailing_slash;
	gchar *ret;

	g_return_val_if_fail (filename != NULL, NULL);

	home_dir_without_trailing_slash = get_home_dir_without_trailing_slash (home_dir);
	if (home_dir_without_trailing_slash == NULL)
	{
		return g_strdup (filename);
	}

	home_dir_with_trailing_slash = g_strdup_printf ("%s/", home_dir_without_trailing_slash);

	if (g_str_equal (filename, home_dir_without_trailing_slash) ||
	    g_str_equal (filename, home_dir_with_trailing_slash))
	{
		ret = g_strdup ("~");
		goto out;
	}

	if (g_str_has_prefix (filename, home_dir_with_trailing_slash))
	{
		ret = g_strdup_printf ("~/%s", filename + strlen (home_dir_with_trailing_slash));
		goto out;
	}

	ret = g_strdup (filename);

out:
	g_free (home_dir_without_trailing_slash);
	g_free (home_dir_with_trailing_slash);
	return ret;
}

/*
 * _amtk_utils_replace_home_dir_with_tilde:
 * @filename: the filename.
 *
 * Replaces the home directory with a tilde, if the home directory is present in
 * the @filename.
 *
 * Returns: the new filename. Free with g_free().
 */
/* This function is a copy from tepl-utils, which originally comes from gedit. */
gchar *
_amtk_utils_replace_home_dir_with_tilde (const gchar *filename)
{
	return _tepl_utils_replace_home_dir_with_tilde_with_param (filename, g_get_home_dir ());
}

/**
 * amtk_utils_remove_mnemonic:
 * @str: a string.
 *
 * Removes the mnemonics from @str. Single underscores are removed, and two
 * consecutive underscores are replaced by one underscore (see the documentation
 * of gtk_label_new_with_mnemonic()).
 *
 * Returns: (transfer full): the new string with the mnemonics removed. Free
 * with g_free() when no longer needed.
 * Since: 5.0
 */
gchar *
amtk_utils_remove_mnemonic (const gchar *str)
{
	gchar *new_str;
	gint str_pos;
	gint new_str_pos = 0;
	gboolean prev_char_is_underscore_skipped = FALSE;

	g_return_val_if_fail (str != NULL, NULL);

	new_str = g_malloc (strlen (str) + 1);

	for (str_pos = 0; str[str_pos] != '\0'; str_pos++)
	{
		gchar cur_char = str[str_pos];

		if (cur_char == '_' && !prev_char_is_underscore_skipped)
		{
			prev_char_is_underscore_skipped = TRUE;
		}
		else
		{
			new_str[new_str_pos++] = cur_char;
			prev_char_is_underscore_skipped = FALSE;
		}
	}

	new_str[new_str_pos] = '\0';
	return new_str;
}

static gint
get_menu_item_position (GtkMenuShell *menu_shell,
			GtkMenuItem  *item)
{
	GList *children;
	GList *l;
	gint pos;
	gboolean found = FALSE;

	children = gtk_container_get_children (GTK_CONTAINER (menu_shell));

	for (l = children, pos = 0; l != NULL; l = l->next, pos++)
	{
		GtkMenuItem *cur_item = l->data;

		if (cur_item == item)
		{
			found = TRUE;
			break;
		}
	}

	g_list_free (children);

	return found ? pos : -1;
}

/**
 * amtk_utils_recent_chooser_menu_get_item_uri:
 * @menu: a #GtkRecentChooserMenu.
 * @item: a #GtkMenuItem.
 *
 * Gets the URI of @item. @item must be a child of @menu. @menu must be a
 * #GtkRecentChooserMenu.
 *
 * This function has been written because the value returned by
 * gtk_recent_chooser_get_current_uri() is not updated when #GtkMenuItem's of a
 * #GtkRecentChooserMenu are selected/deselected.
 *
 * Returns: the URI of @item. Free with g_free() when no longer needed.
 * Since: 2.0
 */
gchar *
amtk_utils_recent_chooser_menu_get_item_uri (GtkRecentChooserMenu *menu,
					     GtkMenuItem          *item)
{
	gint pos;
	gchar **all_uris;
	gsize length;
	gchar *item_uri = NULL;

	g_return_val_if_fail (GTK_IS_RECENT_CHOOSER_MENU (menu), NULL);
	g_return_val_if_fail (GTK_IS_MENU_ITEM (item), NULL);

	{
		GtkWidget *item_parent;

		item_parent = gtk_widget_get_parent (GTK_WIDGET (item));
		g_return_val_if_fail (item_parent == GTK_WIDGET (menu), NULL);
	}

	pos = get_menu_item_position (GTK_MENU_SHELL (menu), item);
	g_return_val_if_fail (pos >= 0, NULL);

	all_uris = gtk_recent_chooser_get_uris (GTK_RECENT_CHOOSER (menu), &length);

	if ((gsize)pos < length)
	{
		item_uri = g_strdup (all_uris[pos]);
	}

	g_strfreev (all_uris);
	return item_uri;
}

static gboolean
variant_type_equal_null_safe (const GVariantType *type1,
			      const GVariantType *type2)
{
	if (type1 == NULL || type2 == NULL)
	{
		return type1 == NULL && type2 == NULL;
	}

	return g_variant_type_equal (type1, type2);
}

#define AMTK_GVARIANT_PARAM_KEY "amtk-gvariant-param-key"

G_GNUC_BEGIN_IGNORE_DEPRECATIONS
static void
gtk_action_activate_cb (GtkAction *gtk_action,
			GAction   *g_action)
{
	GVariant *param;

	param = g_object_get_data (G_OBJECT (gtk_action), AMTK_GVARIANT_PARAM_KEY);
	g_action_activate (g_action, param);
}

/**
 * amtk_utils_bind_g_action_to_gtk_action:
 * @g_action_map: a #GActionMap.
 * @detailed_g_action_name_without_prefix: a detailed #GAction name without the
 *   #GActionMap prefix; the #GAction must be present in @g_action_map.
 * @gtk_action_group: a #GtkActionGroup.
 * @gtk_action_name: a #GtkAction name present in @gtk_action_group.
 *
 * Utility function to be able to port an application gradually to #GAction,
 * when #GtkUIManager and #GtkAction are still used. Porting to #GAction should
 * be the first step.
 *
 * For @detailed_g_action_name_without_prefix, see the
 * g_action_parse_detailed_name() function.  The `"app."` or `"win."` prefix (or
 * any other #GActionMap prefix) must not be included in
 * @detailed_g_action_name_without_prefix. For example a valid
 * @detailed_g_action_name_without_prefix is `"open"` or
 * `"insert-command::foobar"`.
 *
 * The same #GAction can be bound to several #GtkAction's (with different
 * parameter values for the #GAction), but the reverse is not true, one
 * #GtkAction cannot be bound to several #GAction's.
 *
 * This function:
 * - Calls g_action_activate() when the #GtkAction #GtkAction::activate signal
 *   is emitted.
 * - Binds the #GAction #GAction:enabled property to the #GtkAction
 *   #GtkAction:sensitive property. The binding is done with the
 *   %G_BINDING_BIDIRECTIONAL and %G_BINDING_SYNC_CREATE flags, the source is
 *   the #GAction and the target is the #GtkAction.
 *
 * When using this function, you should set the callback to %NULL in the
 * corresponding #GtkActionEntry.
 *
 * Since: 4.0
 */
void
amtk_utils_bind_g_action_to_gtk_action (GActionMap     *g_action_map,
					const gchar    *detailed_g_action_name_without_prefix,
					GtkActionGroup *gtk_action_group,
					const gchar    *gtk_action_name)
{
	gchar *g_action_name = NULL;
	GVariant *target_value = NULL;
	GAction *g_action;
	GtkAction *gtk_action;
	GError *error = NULL;

	g_return_if_fail (G_IS_ACTION_MAP (g_action_map));
	g_return_if_fail (detailed_g_action_name_without_prefix != NULL);
	g_return_if_fail (GTK_IS_ACTION_GROUP (gtk_action_group));
	g_return_if_fail (gtk_action_name != NULL);

	g_action_parse_detailed_name (detailed_g_action_name_without_prefix,
				      &g_action_name,
				      &target_value,
				      &error);

	/* The doc of g_action_parse_detailed_name() doesn't explain if it
	 * returns a floating ref for the GVariant.
	 */
	if (target_value != NULL &&
	    g_variant_is_floating (target_value))
	{
		g_variant_ref_sink (target_value);
	}

	if (error != NULL)
	{
		g_warning ("Error when parsing detailed GAction name '%s': %s",
			   detailed_g_action_name_without_prefix,
			   error->message);

		g_clear_error (&error);
		goto out;
	}

	g_action = g_action_map_lookup_action (g_action_map, g_action_name);
	if (g_action == NULL)
	{
		g_warn_if_reached ();
		goto out;
	}

	/* Sanity check, ensure that the GVariant target has the good type. */
	{
		const GVariantType *g_action_param_type;
		const GVariantType *target_value_type = NULL;

		g_action_param_type = g_action_get_parameter_type (g_action);

		if (target_value != NULL)
		{
			target_value_type = g_variant_get_type (target_value);
		}

		if (!variant_type_equal_null_safe (g_action_param_type, target_value_type))
		{
			g_warn_if_reached ();
			goto out;
		}
	}

	gtk_action = gtk_action_group_get_action (gtk_action_group, gtk_action_name);
	if (gtk_action == NULL)
	{
		g_warn_if_reached ();
		goto out;
	}

	if (target_value != NULL)
	{
		g_object_set_data_full (G_OBJECT (gtk_action),
					AMTK_GVARIANT_PARAM_KEY,
					g_variant_ref (target_value),
					(GDestroyNotify)g_variant_unref);
	}

	g_signal_connect_object (gtk_action,
				 "activate",
				 G_CALLBACK (gtk_action_activate_cb),
				 g_action,
				 0);

	g_object_bind_property (g_action, "enabled",
				gtk_action, "sensitive",
				G_BINDING_BIDIRECTIONAL | G_BINDING_SYNC_CREATE);

out:
	g_free (g_action_name);

	if (target_value != NULL)
	{
		g_variant_unref (target_value);
	}
}

/**
 * amtk_utils_create_gtk_action:
 * @g_action_map: a #GActionMap.
 * @detailed_g_action_name_with_prefix: a detailed #GAction name with the
 *   #GActionMap prefix; the #GAction must be present in @g_action_map.
 * @gtk_action_group: a #GtkActionGroup.
 * @gtk_action_name: the name of the #GtkAction to create and add to
 *   @gtk_action_group.
 *
 * Utility function to be able to port an application gradually to #GAction and
 * #AmtkActionInfo, when #GtkUIManager is still used. This function goes one
 * step further compared to amtk_utils_bind_g_action_to_gtk_action(). With
 * amtk_utils_bind_g_action_to_gtk_action(), only the #GAction must exist. With
 * amtk_utils_create_gtk_action(), both the #GAction and #AmtkActionInfo must
 * exist (so typically you need to convert the #GtkActionEntry's into
 * #AmtkActionInfoEntry's).
 *
 * This function creates a #GtkAction from a #GAction plus its corresponding
 * #AmtkActionInfo.
 *
 * The #GtkAction is created with the information provided by the
 * #AmtkActionInfo (retrieved with amtk_action_info_central_store_lookup() with
 * @detailed_g_action_name_with_prefix as argument). Only the first accelerator
 * is taken into account.
 *
 * Once the #GtkAction is created, it is added to the @gtk_action_group, and
 * amtk_utils_bind_g_action_to_gtk_action() is called.
 *
 * Since: 4.0
 */
void
amtk_utils_create_gtk_action (GActionMap     *g_action_map,
			      const gchar    *detailed_g_action_name_with_prefix,
			      GtkActionGroup *gtk_action_group,
			      const gchar    *gtk_action_name)
{
	AmtkActionInfoCentralStore *central_store;
	AmtkActionInfo *g_action_info;
	GtkAction *gtk_action;
	const gchar * const *accels;
	const gchar *first_accel;
	const gchar *detailed_g_action_name_without_prefix;

	g_return_if_fail (G_IS_ACTION_MAP (g_action_map));
	g_return_if_fail (detailed_g_action_name_with_prefix != NULL);
	g_return_if_fail (GTK_IS_ACTION_GROUP (gtk_action_group));
	g_return_if_fail (gtk_action_name != NULL);

	central_store = amtk_action_info_central_store_get_singleton ();
	g_action_info = amtk_action_info_central_store_lookup (central_store, detailed_g_action_name_with_prefix);

	gtk_action = gtk_action_new (gtk_action_name,
				     amtk_action_info_get_label (g_action_info),
				     amtk_action_info_get_tooltip (g_action_info),
				     NULL);

	gtk_action_set_icon_name (gtk_action, amtk_action_info_get_icon_name (g_action_info));

	accels = amtk_action_info_get_accels (g_action_info);
	first_accel = accels != NULL ? accels[0] : NULL;

	gtk_action_group_add_action_with_accel (gtk_action_group, gtk_action, first_accel);
	g_object_unref (gtk_action);

	detailed_g_action_name_without_prefix = strchr (detailed_g_action_name_with_prefix, '.');
	if (detailed_g_action_name_without_prefix != NULL)
	{
		detailed_g_action_name_without_prefix++;
	}
	else
	{
		detailed_g_action_name_without_prefix = detailed_g_action_name_with_prefix;
	}

	amtk_utils_bind_g_action_to_gtk_action (g_action_map,
						detailed_g_action_name_without_prefix,
						gtk_action_group,
						gtk_action_name);
}
G_GNUC_END_IGNORE_DEPRECATIONS

/**
 * amtk_utils_get_shrinkable_menubar:
 * @menubar: a #GtkMenuBar.
 *
 * This function wraps @menubar into a container, to allow the menubar to shrink
 * below its minimum width.
 *
 * A possible use-case: have two applications side-by-side on a single screen.
 *
 * Returns: (transfer floating): a new widget that contains @menubar.
 * Since: 5.6
 */
GtkWidget *
amtk_utils_get_shrinkable_menubar (GtkMenuBar *menubar)
{
	GtkWidget *viewport;
	GtkWidget *hpaned;

	/* Note, this solution might work with other kinds of widgets than a
	 * GtkMenuBar. But it would require more testing. If a more general
	 * solution is desirable, another function can be added.
	 */

	g_return_val_if_fail (GTK_IS_MENU_BAR (menubar), NULL);

	viewport = gtk_viewport_new (NULL, NULL);
	gtk_widget_show (viewport);
	gtk_viewport_set_shadow_type (GTK_VIEWPORT (viewport), GTK_SHADOW_NONE);

	hpaned = gtk_paned_new (GTK_ORIENTATION_HORIZONTAL);
	gtk_widget_show (hpaned);

	/* Packing */
	gtk_container_add (GTK_CONTAINER (viewport), GTK_WIDGET (menubar));
	gtk_paned_add1 (GTK_PANED (hpaned), viewport);

	return hpaned;
}
