CC=gcc
OPT= -Wall -O3 -lm 


calc_stats_com : calc_stats_com.c bgc_read_utils.c 
	${CC} -Wall -O2 -o $@ -lm $@.c

calc_stats_dpp : calc_stats_dpp.c bgc_read_utils.c 
	${CC} -Wall -O3 -funroll-loops -o $@ -lm $@.c

dump_group_pdata : dump_group_pdata.c bgc_read_utils.c 
	${CC} -Wall -O2 -o $@ -lm $@.c

compare_grp_with_idbe : compare_grp_with_idbe.c bgc_read_utils.c 
	${CC} -Wall -O2 -o $@ -lm $@.c

compare_two_grps : compare_two_grps.c bgc_read_utils.c 
	${CC} -Wall -O2 -o $@ -lm $@.c

bgc_test_header : bgc_header_test.c bgc_read_utils.c
	${CC} -Wall -o $@ $@.c

bgc_test_read_idbe : read_bgc_idbe.c bgc_read_utils.c
	${CC} -Wall -o $@ read_bgc_idbe.c

bgc_test_pdata : bgc_test_pdata.c bgc_read_utils.c
	${CC} -Wall -o $@ read_bgc_pdata.c

bgc_read_header : bgc_read_header.c bgc_read_utils.c
	${CC} -Wall -o $@ $@.c

all : read_test bgc_read_header calc_stats_com calc_stats_dpp
clean :
	-rm -f calc_stats_com calc_stats_dpp compare_grp_idbe compare_two_grps read_idbe_test read_test header_test dump_group_pdata read_bgc_header
