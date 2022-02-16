#include <ngx_http.h>

ngx_module_t ngx_http_error_page_inherit_module;

typedef struct {
    ngx_array_t *error_pages;
    ngx_flag_t inherit;
} ngx_http_error_page_inherit_loc_conf_t;

static char *ngx_conf_set_flag_slot_my(ngx_conf_t *cf, ngx_command_t *cmd, void *conf) {
    ngx_http_error_page_inherit_loc_conf_t *lcf = conf;
    ngx_http_core_loc_conf_t *clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
    lcf->error_pages = clcf->error_pages;
    ngx_http_err_page_t *elts = clcf->error_pages->elts;
    for (ngx_uint_t i = 0; i < clcf->error_pages->nelts; i++) {
        ngx_conf_log_error(NGX_LOG_EMERG, cf, 0, "args[%i] = %V", i, &elts[i].value.value);
    }
    return ngx_conf_set_flag_slot(cf, cmd, conf);
}

static ngx_command_t ngx_http_error_page_inherit_commands[] = {
  { .name = ngx_string("error_page_inherit"),
    .type = NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_HTTP_LIF_CONF|NGX_CONF_TAKE1,
    .set = ngx_conf_set_flag_slot_my,
    .conf = NGX_HTTP_LOC_CONF_OFFSET,
    .offset = offsetof(ngx_http_error_page_inherit_loc_conf_t, inherit),
    .post = NULL },
    ngx_null_command
};

static void *ngx_http_error_page_inherit_create_loc_conf(ngx_conf_t *cf) {
    ngx_http_error_page_inherit_loc_conf_t *lcf = ngx_pcalloc(cf->pool, sizeof(*lcf));
    if (!lcf) return NULL;
    lcf->inherit = NGX_CONF_UNSET;
    return lcf;
}

static char *ngx_http_error_page_inherit_merge_loc_conf(ngx_conf_t *cf, void *parent, void *child) {
    ngx_http_error_page_inherit_loc_conf_t *prev = parent;
    ngx_http_error_page_inherit_loc_conf_t *conf = child;
    ngx_conf_merge_value(conf->inherit, prev->inherit, 0);
    if (!conf->inherit) return NGX_CONF_OK;
    if (!prev->error_pages) return NGX_CONF_OK;
    ngx_http_core_loc_conf_t *clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
    if (!clcf->error_pages && !(clcf->error_pages = ngx_array_create(cf->pool, 4, sizeof(ngx_http_err_page_t)))) return NGX_CONF_ERROR;
    if (prev->error_pages != clcf->error_pages) {
        ngx_http_err_page_t *err, *elts = prev->error_pages->elts;
        for (ngx_uint_t i = 0; i < prev->error_pages->nelts; i++) {
            if (!(err = ngx_array_push(clcf->error_pages))) return NGX_CONF_ERROR;
            *err = elts[i];
            ngx_conf_log_error(NGX_LOG_EMERG, cf, 0, "args[%i] = %V", i, &elts[i].value.value);
        }
    }
    conf->error_pages = clcf->error_pages;
    return NGX_CONF_OK;
}

static ngx_http_module_t ngx_http_error_page_inherit_ctx = {
    .preconfiguration = NULL,
    .postconfiguration = NULL,
    .create_main_conf = NULL,
    .init_main_conf = NULL,
    .create_srv_conf = NULL,
    .merge_srv_conf = NULL,
    .create_loc_conf = ngx_http_error_page_inherit_create_loc_conf,
    .merge_loc_conf = ngx_http_error_page_inherit_merge_loc_conf
};

ngx_module_t ngx_http_error_page_inherit_module = {
    NGX_MODULE_V1,
    .ctx = &ngx_http_error_page_inherit_ctx,
    .commands = ngx_http_error_page_inherit_commands,
    .type = NGX_HTTP_MODULE,
    .init_master = NULL,
    .init_module = NULL,
    .init_process = NULL,
    .init_thread = NULL,
    .exit_thread = NULL,
    .exit_process = NULL,
    .exit_master = NULL,
    NGX_MODULE_V1_PADDING
};
