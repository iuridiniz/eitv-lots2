typedef enum _MdwMessageType MdwMessageType;
/* order: priority, lower first */
/* enum _MdwMessageType {
    MDW_MESSAGE_TYPE_NULL = 0, 
    MDW_MESSAGE_TYPE_PAUSE,
    MDW_MESSAGE_TYPE_READY,
    MDW_MESSAGE_TYPE_DROP_ALL,
    MDW_MESSAGE_TYPE_NEW_PES,
    MDW_MESSAGE_TYPE_NEW_FRAME
};
*/
typedef struct _MdwMessage MdwMessage;
struct _MdwMessage {
  MdwMessageType type;
  guint64 id;
  void* buffer;
  gsize size;
};

static inline guint64
_mdw_message_get_id()
{
    static guint64 id = 0;
    return id++;
}

static gint 
_mdw_message_sort (const MdwMessage *m1, const MdwMessage *m2, void* nil) 
{
    return m1->type - m2->type;
}

static inline MdwMessage* 
mdw_message_new(MdwMessageType type, gsize segment_size) 
{
    MdwMessage* m = g_new0(MdwMessage, 1);
    m->type = type;
    m->id = _mdw_message_get_id();
    if (segment_size) {
        m->buffer = (void*) g_new(guint8, segment_size);
        m->size = segment_size;
    }
    return m;
}

static inline void
mdw_message_put(MdwMessage* message, GAsyncQueue* queue) 
{
    g_return_if_fail(message != NULL);
    g_return_if_fail(queue != NULL);
    g_async_queue_push_sorted(queue, (gpointer) message, (GCompareDataFunc) _mdw_message_sort, NULL);
}

static inline void
mdw_message_free(MdwMessage* message, gboolean free_segment) {
    g_return_if_fail(message != NULL);
    if (free_segment && message->buffer != NULL) {
        g_assert(message->size > 0);
        g_free(message->buffer);
        //message->buffer = NULL;
        //message->size = 0;
    }
    g_free(message);
}

static inline MdwMessage*
mdw_message_get(GAsyncQueue* queue, gboolean block) {
    if (block) {
        return (MdwMessage*) g_async_queue_pop(queue);
    }
    return (MdwMessage*) g_async_queue_try_pop(queue);
}

static inline MdwMessage*
mdw_message_get_unlocked(GAsyncQueue* queue, gboolean block) {
    if (block) {
        return (MdwMessage*) g_async_queue_pop_unlocked(queue);
    }
    return (MdwMessage*) g_async_queue_try_pop_unlocked(queue);
}
