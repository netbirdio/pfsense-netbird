#!/bin/sh
#
# NetBird Interface Pre-creation Script
#
# This script pre-creates the wt0 interface early at boot. When NetBird starts
# later, it takes ownership of the existing interface. The interface name (wt0)
# must match NetBird's default. This script is idempotent and safe to run
# multiple times.
#

# Check if wt0 already exists; if not, create it
if ! /sbin/ifconfig wt0 >/dev/null 2>&1; then
    /sbin/ifconfig tun create name wt0 >/dev/null 2>&1
    logger -t netbird "Pre-created wt0 interface for NetBird"
fi

exit 0