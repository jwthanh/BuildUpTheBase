from __future__ import with_statement

from fabric.api import local, settings, abort, run, cd, env, put
from fabric.contrib.console import confirm

from fab_config import HOSTS, TEMPLATE_DIR

env.hosts = HOSTS

def build_changelog():
    print "getting changelog"
    local("python python/list_all_recent_changes.py com.tankorsmash.buildupthebase > changelog.txt")

def update_changelog():
    build_changelog()

    print "putting changelog on server"
    with cd(TEMPLATE_DIR):
        run("ls")
        put("changelog.txt", "index.html")
