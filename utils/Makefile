CC=gcc
CFLAGS= -Wall -O3 -lm 
MOPT= -funroll-loops
# READING LIBRARIES
RLIBS=bgc.h bgc_read_utils.c
WLIBS=bgc.h bgc_write_utils.c
# first, let's list the useful utilities
all : bgc_read_header bgc_test_pdata bgc_stats_com bgc_stats_dpp 

bgc_read_header : bgc_read_header.c $(RLIBS)
	${CC} $(CFLAGS) -o $@ $@.c

bgc_test_pdata : bgc_test_pdata.c $(RLIBS)
	${CC} $(CFLAGS) -o $@ $@.c

bgc_stats_com : bgc_stats_com.c $(RLIBS) 
	${CC} $(CFLAGS) -o $@ $@.c

bgc_stats_dpp : bgc_stats_dpp.c $(RLIBS) 
	${CC} $(CFLAGS) $(MOPT) -o $@ $@.c

# the rest of these are testing, old examples or more specific utilites
bgc_header_size: bgc_header_size.c $(RLIBS) 
	${CC} $(CFLAGS) -o $@ $@.c

bgc_dump_group_pdata : bgc_dump_group_pdata.c $(RLIBS) 
	${CC} $(CFLAGS) -o $@ $@.c

bgc_readsave_idbe : bgc_readsave_idbe.c $(RLIBS) 
	${CC} $(CFLAGS) -o $@ $@.c

bgc_copy_test: bgc_copy_test.c $(RLIBS) $(WLIBS)
	${CC} $(CFLAGS) -o $@ $@.c

compare_grp_with_id : compare_grp_with_id.c $(RLIBS)
	${CC} $(CFLAGS) -o $@ $@.c

compare_grp_with_idbe : compare_grp_with_idbe.c $(RLIBS)
	${CC} $(CFLAGS) -o $@ $@.c

compare_two_grps : compare_two_grps.c 
	${CC} $(CFLAGS) -o $@ $@.c

everything: all bgc_header_size bgc_dump_group_pdata bgc_readsave_idbe bgc_copy_test \
            compare_grp_with_id compare_grp_with_idbe compare_two_grps

clean :
	-$(RM) bgc_read_header bgc_test_pdata bgc_stats_com bgc_stats_dpp \
	       bgc_header_size bgc_dump_group_pdata bgc_readsave_idbe bgc_copy_test \
	       compare_grp_with_id compare_grp_with_idbe compare_two_grps

