

#include "config.h"

#include "webkitwebhistoryitem.h"
#include "webkitprivate.h"

#include <glib.h>
#include <glib/gi18n-lib.h>

#include "CString.h"
#include "HistoryItem.h"
#include "PlatformString.h"


using namespace WebKit;

struct _WebKitWebHistoryItemPrivate {
    WebCore::HistoryItem* historyItem;

    WebCore::CString title;
    WebCore::CString alternateTitle;
    WebCore::CString uri;
    WebCore::CString originalUri;

    gboolean disposed;
};

#define WEBKIT_WEB_HISTORY_ITEM_GET_PRIVATE(obj)    (G_TYPE_INSTANCE_GET_PRIVATE((obj), WEBKIT_TYPE_WEB_HISTORY_ITEM, WebKitWebHistoryItemPrivate))

enum {
    PROP_0,

    PROP_TITLE,
    PROP_ALTERNATE_TITLE,
    PROP_URI,
    PROP_ORIGINAL_URI,
    PROP_LAST_VISITED_TIME
};

G_DEFINE_TYPE(WebKitWebHistoryItem, webkit_web_history_item, G_TYPE_OBJECT);

static void webkit_web_history_item_set_property(GObject* object, guint prop_id, const GValue* value, GParamSpec* pspec);

static void webkit_web_history_item_get_property(GObject* object, guint prop_id, GValue* value, GParamSpec* pspec);

GHashTable* webkit_history_items()
{
    static GHashTable* historyItems = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, g_object_unref);
    return historyItems;
}

void webkit_history_item_add(WebKitWebHistoryItem* webHistoryItem, WebCore::HistoryItem* historyItem)
{
    g_return_if_fail(WEBKIT_IS_WEB_HISTORY_ITEM(webHistoryItem));

    GHashTable* table = webkit_history_items();
    g_hash_table_insert(table, historyItem, webHistoryItem);
}

static void webkit_web_history_item_dispose(GObject* object)
{
    WebKitWebHistoryItem* webHistoryItem = WEBKIT_WEB_HISTORY_ITEM(object);
    WebKitWebHistoryItemPrivate* priv = webHistoryItem->priv;

    if (!priv->disposed) {
        WebCore::HistoryItem* item = core(webHistoryItem);
        item->deref();
        priv->disposed = true;
    }

    G_OBJECT_CLASS(webkit_web_history_item_parent_class)->dispose(object);
}

static void webkit_web_history_item_finalize(GObject* object)
{
    WebKitWebHistoryItem* webHistoryItem = WEBKIT_WEB_HISTORY_ITEM(object);
    WebKitWebHistoryItemPrivate* priv = webHistoryItem->priv;

    priv->title = WebCore::CString();
    priv->alternateTitle = WebCore::CString();
    priv->uri = WebCore::CString();
    priv->originalUri = WebCore::CString();

    G_OBJECT_CLASS(webkit_web_history_item_parent_class)->finalize(object);
}

static void webkit_web_history_item_class_init(WebKitWebHistoryItemClass* klass)
{
    GObjectClass* gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose = webkit_web_history_item_dispose;
    gobject_class->finalize = webkit_web_history_item_finalize;
    gobject_class->set_property = webkit_web_history_item_set_property;
    gobject_class->get_property = webkit_web_history_item_get_property;

    webkit_init();

    /**
    * WebKitWebHistoryItem:title:
    *
    * The title of the history item.
    *
    * Since: 1.0.2
    */
    g_object_class_install_property(gobject_class,
                                    PROP_TITLE,
                                    g_param_spec_string(
                                    "title",
                                    _("Title"),
                                    _("The title of the history item"),
                                    NULL,
                                    WEBKIT_PARAM_READABLE));

    /**
    * WebKitWebHistoryItem:alternate-title:
    *
    * The alternate title of the history item.
    *
    * Since: 1.0.2
    */
    g_object_class_install_property(gobject_class,
                                    PROP_ALTERNATE_TITLE,
                                    g_param_spec_string(
                                    "alternate-title",
                                    _("Alternate Title"),
                                    _("The alternate title of the history item"),
                                    NULL,
                                    WEBKIT_PARAM_READWRITE));

    /**
    * WebKitWebHistoryItem:uri:
    *
    * The URI of the history item.
    *
    * Since: 1.0.2
    */
    g_object_class_install_property(gobject_class,
                                    PROP_URI,
                                    g_param_spec_string(
                                    "uri",
                                    _("URI"),
                                    _("The URI of the history item"),
                                    NULL,
                                    WEBKIT_PARAM_READABLE));

    /**
    * WebKitWebHistoryItem:original-uri:
    *
    * The original URI of the history item.
    *
    * Since: 1.0.2
    */
    g_object_class_install_property(gobject_class,
                                    PROP_ORIGINAL_URI,
                                    g_param_spec_string(
                                    "original-uri",
                                    _("Original URI"),
                                    _("The original URI of the history item"),
                                    NULL,
                                    WEBKIT_PARAM_READABLE));

   /**
    * WebKitWebHistoryItem:last-visited-time:
    *
    * The time at which the history item was last visited.
    *
    * Since: 1.0.2
    */
    g_object_class_install_property(gobject_class,
                                    PROP_LAST_VISITED_TIME,
                                    g_param_spec_double(
                                    "last-visited-time",
                                    _("Last visited Time"),
                                    _("The time at which the history item was last visited"),
                                    0, G_MAXDOUBLE, 0,
                                    WEBKIT_PARAM_READABLE));

    g_type_class_add_private(gobject_class, sizeof(WebKitWebHistoryItemPrivate));
}

static void webkit_web_history_item_init(WebKitWebHistoryItem* webHistoryItem)
{
    webHistoryItem->priv = WEBKIT_WEB_HISTORY_ITEM_GET_PRIVATE(webHistoryItem);
}

static void webkit_web_history_item_set_property(GObject* object, guint prop_id, const GValue* value, GParamSpec* pspec)
{
    WebKitWebHistoryItem* webHistoryItem = WEBKIT_WEB_HISTORY_ITEM(object);

    switch(prop_id) {
    case PROP_ALTERNATE_TITLE:
        webkit_web_history_item_set_alternate_title(webHistoryItem, g_value_get_string(value));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void webkit_web_history_item_get_property(GObject* object, guint prop_id, GValue* value, GParamSpec* pspec)
{
    WebKitWebHistoryItem* webHistoryItem = WEBKIT_WEB_HISTORY_ITEM(object);

    switch (prop_id) {
    case PROP_TITLE:
        g_value_set_string(value, webkit_web_history_item_get_title(webHistoryItem));
        break;
    case PROP_ALTERNATE_TITLE:
        g_value_set_string(value, webkit_web_history_item_get_alternate_title(webHistoryItem));
        break;
    case PROP_URI:
        g_value_set_string(value, webkit_web_history_item_get_uri(webHistoryItem));
        break;
    case PROP_ORIGINAL_URI:
        g_value_set_string(value, webkit_web_history_item_get_original_uri(webHistoryItem));
        break;
    case PROP_LAST_VISITED_TIME:
        g_value_set_double(value, webkit_web_history_item_get_last_visited_time(webHistoryItem));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

/* Helper function to create a new WebHistoryItem instance when needed */
WebKitWebHistoryItem* webkit_web_history_item_new_with_core_item(PassRefPtr<WebCore::HistoryItem> historyItem)
{
    return kit(historyItem);
}


WebKitWebHistoryItem* webkit_web_history_item_new()
{
    WebKitWebHistoryItem* webHistoryItem = WEBKIT_WEB_HISTORY_ITEM(g_object_new(WEBKIT_TYPE_WEB_HISTORY_ITEM, NULL));
    WebKitWebHistoryItemPrivate* priv = webHistoryItem->priv;

    RefPtr<WebCore::HistoryItem> item = WebCore::HistoryItem::create();
    priv->historyItem = item.release().releaseRef();
    webkit_history_item_add(webHistoryItem, priv->historyItem);

    return webHistoryItem;
}

WebKitWebHistoryItem* webkit_web_history_item_new_with_data(const gchar* uri, const gchar* title)
{
    WebKitWebHistoryItem* webHistoryItem = WEBKIT_WEB_HISTORY_ITEM(g_object_new(WEBKIT_TYPE_WEB_HISTORY_ITEM, NULL));
    WebKitWebHistoryItemPrivate* priv = webHistoryItem->priv;

    WebCore::KURL historyUri(WebCore::KURL(), uri);
    WebCore::String historyTitle = WebCore::String::fromUTF8(title);
    RefPtr<WebCore::HistoryItem> item = WebCore::HistoryItem::create(historyUri, historyTitle, 0);
    priv->historyItem = item.release().releaseRef();
    webkit_history_item_add(webHistoryItem, priv->historyItem);

    return webHistoryItem;
}

G_CONST_RETURN gchar* webkit_web_history_item_get_title(WebKitWebHistoryItem* webHistoryItem)
{
    g_return_val_if_fail(WEBKIT_IS_WEB_HISTORY_ITEM(webHistoryItem), NULL);

    WebCore::HistoryItem* item = core(webHistoryItem);

    g_return_val_if_fail(item, NULL);

    WebKitWebHistoryItemPrivate* priv = webHistoryItem->priv;
    priv->title = item->title().utf8();

    return priv->title.data();
}

G_CONST_RETURN gchar* webkit_web_history_item_get_alternate_title(WebKitWebHistoryItem* webHistoryItem)
{
    g_return_val_if_fail(WEBKIT_IS_WEB_HISTORY_ITEM(webHistoryItem), NULL);

    WebCore::HistoryItem* item = core(webHistoryItem);

    g_return_val_if_fail(item, NULL);

    WebKitWebHistoryItemPrivate* priv = webHistoryItem->priv;
    priv->alternateTitle = item->alternateTitle().utf8();

    return priv->alternateTitle.data();
}

void webkit_web_history_item_set_alternate_title(WebKitWebHistoryItem* webHistoryItem, const gchar* title)
{
    g_return_if_fail(WEBKIT_IS_WEB_HISTORY_ITEM(webHistoryItem));
    g_return_if_fail(title);

    WebCore::HistoryItem* item = core(webHistoryItem);

    item->setAlternateTitle(WebCore::String::fromUTF8(title));
    g_object_notify(G_OBJECT(webHistoryItem), "alternate-title");
}

G_CONST_RETURN gchar* webkit_web_history_item_get_uri(WebKitWebHistoryItem* webHistoryItem)
{
    g_return_val_if_fail(WEBKIT_IS_WEB_HISTORY_ITEM(webHistoryItem), NULL);

    WebCore::HistoryItem* item = core(WEBKIT_WEB_HISTORY_ITEM(webHistoryItem));

    g_return_val_if_fail(item, NULL);

    WebKitWebHistoryItemPrivate* priv = webHistoryItem->priv;
    priv->uri = item->urlString().utf8();

    return priv->uri.data();
}

G_CONST_RETURN gchar* webkit_web_history_item_get_original_uri(WebKitWebHistoryItem* webHistoryItem)
{
    g_return_val_if_fail(WEBKIT_IS_WEB_HISTORY_ITEM(webHistoryItem), NULL);

    WebCore::HistoryItem* item = core(WEBKIT_WEB_HISTORY_ITEM(webHistoryItem));

    g_return_val_if_fail(item, NULL);

    WebKitWebHistoryItemPrivate* priv = webHistoryItem->priv;
    priv->originalUri = item->originalURLString().utf8();

    return webHistoryItem->priv->originalUri.data();
}

gdouble webkit_web_history_item_get_last_visited_time(WebKitWebHistoryItem* webHistoryItem)
{
    g_return_val_if_fail(WEBKIT_IS_WEB_HISTORY_ITEM(webHistoryItem), 0);

    WebCore::HistoryItem* item = core(WEBKIT_WEB_HISTORY_ITEM(webHistoryItem));

    g_return_val_if_fail(item, 0);

    return item->lastVisitedTime();
}

WebKitWebHistoryItem* webkit_web_history_item_copy(WebKitWebHistoryItem* self)
{
    WebKitWebHistoryItemPrivate* selfPrivate = self->priv;

    WebKitWebHistoryItem* item = WEBKIT_WEB_HISTORY_ITEM(g_object_new(WEBKIT_TYPE_WEB_HISTORY_ITEM, 0));
    WebKitWebHistoryItemPrivate* priv = item->priv;

    priv->title = selfPrivate->title;
    priv->alternateTitle = selfPrivate->alternateTitle;
    priv->uri = selfPrivate->uri;
    priv->originalUri = selfPrivate->originalUri;

    priv->historyItem = selfPrivate->historyItem->copy().releaseRef();

    return item;
}

/* private methods */

G_CONST_RETURN gchar* webkit_web_history_item_get_target(WebKitWebHistoryItem* webHistoryItem)
{
    g_return_val_if_fail(WEBKIT_IS_WEB_HISTORY_ITEM(webHistoryItem), NULL);

    WebCore::HistoryItem* item = core(webHistoryItem);

    g_return_val_if_fail(item, NULL);

    WebCore::CString t = item->target().utf8();
    return g_strdup(t.data());
}

gboolean webkit_web_history_item_is_target_item(WebKitWebHistoryItem* webHistoryItem)
{
    g_return_val_if_fail(WEBKIT_IS_WEB_HISTORY_ITEM(webHistoryItem), false);

    WebCore::HistoryItem* item = core(webHistoryItem);

    g_return_val_if_fail(item, false);

    return item->isTargetItem();
}

GList* webkit_web_history_item_get_children(WebKitWebHistoryItem* webHistoryItem)
{
    g_return_val_if_fail(WEBKIT_IS_WEB_HISTORY_ITEM(webHistoryItem), NULL);

    WebCore::HistoryItem* item = core(webHistoryItem);

    g_return_val_if_fail(item, NULL);

    const WebCore::HistoryItemVector& children = item->children();
    if (!children.size())
        return NULL;

    unsigned size = children.size();
    GList* kids = NULL;
    for (unsigned i = 0; i < size; ++i)
        kids = g_list_prepend(kids, kit(children[i].get()));

    return g_list_reverse(kids);
}

WebCore::HistoryItem* WebKit::core(WebKitWebHistoryItem* webHistoryItem)
{
    g_return_val_if_fail(WEBKIT_IS_WEB_HISTORY_ITEM(webHistoryItem), NULL);

    return webHistoryItem->priv->historyItem;
}

WebKitWebHistoryItem* WebKit::kit(PassRefPtr<WebCore::HistoryItem> historyItem)
{
    g_return_val_if_fail(historyItem, NULL);

    RefPtr<WebCore::HistoryItem> item = historyItem;
    GHashTable* table = webkit_history_items();
    WebKitWebHistoryItem* webHistoryItem = (WebKitWebHistoryItem*) g_hash_table_lookup(table, item.get());

    if (!webHistoryItem) {
        webHistoryItem = WEBKIT_WEB_HISTORY_ITEM(g_object_new(WEBKIT_TYPE_WEB_HISTORY_ITEM, NULL));
        WebKitWebHistoryItemPrivate* priv = webHistoryItem->priv;

        priv->historyItem = item.release().releaseRef();
        webkit_history_item_add(webHistoryItem, priv->historyItem);
    }

    return webHistoryItem;
}
