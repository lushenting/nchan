#ifndef NCHAN_REDIS_NODESET_H
#define NCHAN_REDIS_NODESET_H

#include <nchan_module.h>
#include "store-private.h"

#define NCHAN_MAX_NODESETS 1024

redis_nodeset_t *nodeset_create(nchan_redis_conf_t *rcf);
redis_nodeset_t *nodeset_find(nchan_redis_conf_t *rcf);
ngx_int_t nodeset_node_destroy(redis_node_t *node);

redis_node_t *nodeset_node_find(redis_nodeset_t *ns, redis_connect_params_t *rcp);
redis_node_t *nodeset_node_create(redis_nodeset_t *ns, redis_connect_params_t *rcp);



#endif /* NCHAN_REDIS_NODESET_H */