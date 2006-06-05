#! /bin/sh

PATH=/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin
DAEMON=/usr/sbin/cupsd
NAME=cupsd
PIDFILE=/var/run/cups/$NAME.pid
DESC="Common Unix Printing System"

unset TMPDIR

test -f $DAEMON || exit 0

set -e

. /lib/lsb/init-functions

# Get the timezone set.
if [ -z "$TZ" -a -e /etc/timezone ]; then
    TZ=`cat /etc/timezone`
    export TZ
fi

case "$1" in
  start)
	log_begin_msg "Starting $DESC: $NAME"
	chown root:lpadmin /usr/share/cups/model 2>/dev/null || true
	chmod 3775 /usr/share/cups/model 2>/dev/null || true
	mkdir -p `dirname "$PIDFILE"`
	start-stop-daemon --start --quiet --oknodo --pidfile "$PIDFILE" --exec $DAEMON
	log_end_msg $?
	;;
  stop)
	log_begin_msg "Stopping $DESC: $NAME"
	start-stop-daemon --stop --quiet --retry 5 --oknodo --pidfile $PIDFILE --name $NAME
	log_end_msg $?
	;;
  reload|force-reload)
	log_begin_msg "Reloading $DESC: $NAME"
	start-stop-daemon --stop --quiet --pidfile $PIDFILE --name $NAME --signal 1
	log_end_msg $?
	;;
  restart)
	log_begin_msg "Restarting $DESC: $NAME"
	if start-stop-daemon --stop --quiet --retry 5 --oknodo --pidfile $PIDFILE --name $NAME; then
		start-stop-daemon --start --quiet --pidfile "$PIDFILE" --exec $DAEMON
	fi
	log_end_msg $?
	;;
  status)
	echo -n "Status of $DESC: "
	if [ ! -r "$PIDFILE" ]; then
		echo "$NAME is not running."
		exit 3
	fi
	if read pid < "$PIDFILE" && ps -p "$pid" > /dev/null 2>&1; then
		echo "$NAME is running."
		exit 0
	else
		echo "$NAME is not running but $PIDFILE exists."
		exit 1
	fi
	;;
  *)
	N=/etc/init.d/${0##*/}
	echo "Usage: $N {start|stop|restart|force-reload|status}" >&2
	exit 1
	;;
esac

exit 0
