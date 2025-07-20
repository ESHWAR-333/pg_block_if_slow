#include "postgres.h"
#include "fmgr.h"
#include "tcop/utility.h"
#include <float.h>
#include "tcop/tcopprot.h"
#include "optimizer/planner.h"
#include "commands/explain.h"
#include "utils/guc.h"

PlannedStmt *block_if_slow_planner(Query *parse, const char *query_string, int cursorOptions, ParamListInfo boundParams);
void _PG_init(void);
void _PG_fini(void);

PG_MODULE_MAGIC;

static planner_hook_type prev_planner_hook = NULL;

double pg_block_if_slow_threshold = 100000.0;

// Custom planner hook
PlannedStmt *block_if_slow_planner(Query *parse, const char *query_string, int cursorOptions, ParamListInfo boundParams)
{
	    PlannedStmt *result;
	    ExplainState *es;

            if (prev_planner_hook)
		result = prev_planner_hook(parse, query_string, cursorOptions, boundParams);
   	    else
		result = standard_planner(parse, query_string, cursorOptions, boundParams);

            es = NewExplainState();
            es->costs = true;
	    es->format = EXPLAIN_FORMAT_TEXT;
	    if (result->planTree->total_cost > pg_block_if_slow_threshold)
	        ereport(ERROR, 
				(errcode(ERRCODE_QUERY_CANCELED),
				errmsg("Query blocked: estimated cost %.2f exceeds threshold %.2f",
			        result->planTree->total_cost,
				pg_block_if_slow_threshold)));

	    return result;
}


void _PG_init(void)
{
DefineCustomRealVariable("pg_block_if_slow.threshold",
		          "Set the maximum allowed estimated total cost.",
			   NULL,
			   &pg_block_if_slow_threshold,
			   100000.0,
			   0,
			   DBL_MAX,
			   PGC_USERSET,
			   0,
			   NULL,
			   NULL,
			   NULL);
prev_planner_hook = planner_hook;
planner_hook = block_if_slow_planner;


}

void _PG_fini(void)
{
	    planner_hook = prev_planner_hook;
} 


