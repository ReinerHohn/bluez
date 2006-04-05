#ifndef __GLIB_ECTOMY_H
#define __GLIB_ECTOMY_H

#include <stdlib.h>
#include <sys/poll.h>

typedef char	gchar;
typedef short	gshort;
typedef long	glong;
typedef int	gint;
typedef gint	gboolean;

typedef unsigned char	guchar;
typedef unsigned short	gushort;
typedef unsigned long	gulong;
typedef unsigned int	guint;

typedef float	gfloat;
typedef double	gdouble;

typedef void *		gpointer;
typedef const void *	gconstpointer;

typedef size_t	gsize;
typedef ssize_t	gssize;

#ifndef SSIZE_MAX
#define SSIZE_MAX	INT_MAX
#endif

#define MIN_TIMEOUT(a, b)  (((a) < (b)) ? (a) : (b))

typedef struct _GIOChannel {
	int fd;
} GIOChannel;

typedef int (timeout_func_t)(void *data);

typedef struct {
	glong tv_sec;
	glong tv_usec;
} time_val_t;

struct timeout {
	guint id;
	guint interval;
	time_val_t expiration;
	void *data;
	timeout_func_t *func;
};

typedef struct _GMainContext {
	guint next_id;
	glong timeout;
	struct slist *ltimeout;
} GMainContext;

typedef struct _GMainLoop {
	int bail;
	GMainContext *context;
} GMainLoop;

typedef enum {
	G_IO_ERROR_NONE,
	G_IO_ERROR_AGAIN,
	G_IO_ERROR_INVAL,
	G_IO_ERROR_UNKNOWN
} GIOError;

typedef enum {
	G_IO_STATUS_ERROR	= -1,
	G_IO_STATUS_NORMAL	= 0,
	G_IO_STATUS_EOF		= 1,
	G_IO_STATUS_AGAIN	= 2
} GIOStatus;

#ifndef FALSE
#define FALSE (0)
#endif

#ifndef TRUE
#define TRUE (!FALSE)
#endif

typedef enum {
	G_IO_IN		= POLLIN,
	G_IO_OUT	= POLLOUT,
	G_IO_PRI	= POLLPRI,
	G_IO_ERR	= POLLERR,
	G_IO_HUP	= POLLHUP,
	G_IO_NVAL	= POLLNVAL
} GIOCondition;

typedef void (*GDestroyNotify) (gpointer data);
typedef gboolean (*GIOFunc) (GIOChannel *source, GIOCondition condition, gpointer data);

GIOError g_io_channel_read(GIOChannel *channel, gchar *buf, gsize count, gsize *bytes_read);
void g_io_channel_close(GIOChannel *channel);

GIOChannel *g_io_channel_unix_new(int fd);
gint g_io_channel_unix_get_fd(GIOChannel *channel);
guint g_io_add_watch(GIOChannel *channel, GIOCondition condition,
					GIOFunc func, gpointer user_data);
guint g_io_add_watch_full(GIOChannel *channel, gint priority,
				GIOCondition condition, GIOFunc func,
				gpointer user_data, GDestroyNotify notify);
void g_io_remove_watch(guint id);

GMainLoop *g_main_loop_new(GMainContext *context, gboolean is_running);
void g_main_loop_run(GMainLoop *loop);
void g_main_loop_quit(GMainLoop *loop);
void g_main_loop_unref(GMainLoop *loop);
guint g_timeout_add(guint interval, timeout_func_t *func, void *data);
gint g_timeout_remove(const guint id);


#define g_main_new(is_running)	g_main_loop_new(NULL, is_running);
#define g_main_run(loop)	g_main_loop_run(loop)
#define g_main_quit(loop)	g_main_loop_quit(loop)
#define g_main_unref(loop)	g_main_loop_unref(loop)

#endif /* __GLIB_ECTOMY_H */
