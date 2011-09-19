SUBDIRS=src examples

if HAVE_CHECK
SUBDIRS += unittest
endif

apidoc: Doxyfile
		(cat Doxyfile; echo "PROJECT_NUMBER=rev `svn info | grep 'Last Changed Rev:' | awk '{print $$4}'`") | doxygen -
