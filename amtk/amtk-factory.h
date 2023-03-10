/* SPDX-FileCopyrightText: 2017, 2018, 2020 - Sébastien Wilmet <swilmet@gnome.org>
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef AMTK_FACTORY_H
#define AMTK_FACTORY_H

#if !defined (AMTK_H_INSIDE) && !defined (AMTK_COMPILATION)
#error "Only <amtk/amtk.h> can be included directly."
#endif

#include <gtk/gtk.h>
#include <amtk/amtk-action-info.h>

G_BEGIN_DECLS

#define AMTK_TYPE_FACTORY             (amtk_factory_get_type ())
#define AMTK_FACTORY(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), AMTK_TYPE_FACTORY, AmtkFactory))
#define AMTK_FACTORY_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), AMTK_TYPE_FACTORY, AmtkFactoryClass))
#define AMTK_IS_FACTORY(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AMTK_TYPE_FACTORY))
#define AMTK_IS_FACTORY_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), AMTK_TYPE_FACTORY))
#define AMTK_FACTORY_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), AMTK_TYPE_FACTORY, AmtkFactoryClass))

typedef struct _AmtkFactory         AmtkFactory;
typedef struct _AmtkFactoryClass    AmtkFactoryClass;
typedef struct _AmtkFactoryPrivate  AmtkFactoryPrivate;

struct _AmtkFactory
{
	GObject parent;

	AmtkFactoryPrivate *priv;
};

struct _AmtkFactoryClass
{
	GObjectClass parent_class;

	gpointer padding[12];
};

/**
 * AmtkFactoryFlags:
 * @AMTK_FACTORY_FLAGS_NONE: No flags.
 * @AMTK_FACTORY_IGNORE_GACTION: Do not associate the created object with the
 *   #GAction. For example if the object to create is a #GtkActionable, do not
 *   call gtk_actionable_set_detailed_action_name().
 * @AMTK_FACTORY_IGNORE_ICON: Do not set an icon.
 * @AMTK_FACTORY_IGNORE_LABEL: Do not set a label/short description.
 * @AMTK_FACTORY_IGNORE_TOOLTIP: Do not set a tooltip/long description.
 * @AMTK_FACTORY_IGNORE_ACCELS: Ignore completely the accelerators.
 * @AMTK_FACTORY_IGNORE_ACCELS_FOR_DOC: Ignore the accelerators for
 *   documentation purposes only. For example do not add/configure a
 *   #GtkAccelLabel.
 * @AMTK_FACTORY_IGNORE_ACCELS_FOR_APP: Do not call
 *   gtk_application_set_accels_for_action().
 *
 * #AmtkFactoryFlags permits to control how a factory function creates the
 * object, to ignore some steps.
 *
 * Since: 3.0
 */
typedef enum
{
	AMTK_FACTORY_FLAGS_NONE			= 0,
	AMTK_FACTORY_IGNORE_GACTION		= 1 << 0,
	AMTK_FACTORY_IGNORE_ICON		= 1 << 1,
	AMTK_FACTORY_IGNORE_LABEL		= 1 << 2,
	AMTK_FACTORY_IGNORE_TOOLTIP		= 1 << 3,
	AMTK_FACTORY_IGNORE_ACCELS		= 1 << 4,
	AMTK_FACTORY_IGNORE_ACCELS_FOR_DOC	= 1 << 5,
	AMTK_FACTORY_IGNORE_ACCELS_FOR_APP	= 1 << 6,
} AmtkFactoryFlags;

G_MODULE_EXPORT
GType			amtk_factory_get_type				(void);

G_MODULE_EXPORT
AmtkFactory *		amtk_factory_new				(GtkApplication *application);

G_MODULE_EXPORT
AmtkFactory *		amtk_factory_new_with_default_application	(void);

G_MODULE_EXPORT
GtkApplication *	amtk_factory_get_application			(AmtkFactory *factory);

G_MODULE_EXPORT
AmtkFactoryFlags	amtk_factory_get_default_flags			(AmtkFactory *factory);

G_MODULE_EXPORT
void			amtk_factory_set_default_flags			(AmtkFactory      *factory,
									 AmtkFactoryFlags  default_flags);

G_MODULE_EXPORT
GtkWidget *		amtk_factory_create_menu_item			(AmtkFactory *factory,
									 const gchar *action_name);

G_MODULE_EXPORT
GtkWidget *		amtk_factory_create_menu_item_full		(AmtkFactory      *factory,
									 const gchar      *action_name,
									 AmtkFactoryFlags  flags);

G_MODULE_EXPORT
GtkWidget *		amtk_factory_create_check_menu_item		(AmtkFactory *factory,
									 const gchar *action_name);

G_MODULE_EXPORT
GtkWidget *		amtk_factory_create_check_menu_item_full	(AmtkFactory      *factory,
									 const gchar      *action_name,
									 AmtkFactoryFlags  flags);

G_MODULE_EXPORT
GtkWidget *		amtk_factory_create_simple_menu			(AmtkFactory               *factory,
									 const AmtkActionInfoEntry *entries,
									 gint                       n_entries);

G_MODULE_EXPORT
GtkWidget *		amtk_factory_create_simple_menu_full		(AmtkFactory               *factory,
									 const AmtkActionInfoEntry *entries,
									 gint                       n_entries,
									 AmtkFactoryFlags           flags);

G_MODULE_EXPORT
GMenuItem *		amtk_factory_create_gmenu_item			(AmtkFactory *factory,
									 const gchar *action_name);

G_MODULE_EXPORT
GMenuItem *		amtk_factory_create_gmenu_item_full		(AmtkFactory      *factory,
									 const gchar      *action_name,
									 AmtkFactoryFlags  flags);

G_MODULE_EXPORT
GtkToolItem *		amtk_factory_create_tool_button			(AmtkFactory *factory,
									 const gchar *action_name);

G_MODULE_EXPORT
GtkToolItem *		amtk_factory_create_tool_button_full		(AmtkFactory      *factory,
									 const gchar      *action_name,
									 AmtkFactoryFlags  flags);

G_MODULE_EXPORT
GtkMenuToolButton *	amtk_factory_create_menu_tool_button		(AmtkFactory *factory,
									 const gchar *action_name);

G_MODULE_EXPORT
GtkMenuToolButton *	amtk_factory_create_menu_tool_button_full	(AmtkFactory      *factory,
									 const gchar      *action_name,
									 AmtkFactoryFlags  flags);

G_MODULE_EXPORT
GtkWidget *		amtk_factory_create_shortcut			(AmtkFactory *factory,
									 const gchar *action_name);

G_MODULE_EXPORT
GtkWidget *		amtk_factory_create_shortcut_full		(AmtkFactory      *factory,
									 const gchar      *action_name,
									 AmtkFactoryFlags  flags);

G_END_DECLS

#endif /* AMTK_FACTORY_H */
