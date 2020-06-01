#ifndef __MAIN_H__
#define __MAIN_H__
//#define  HTTP_DEBUG

#define HTTP_HOST "halalla.cn"
//"halalla.cn"
#define PORT "443"
//"443"

#define HTTP_USER_AGENT "Mozilla/5.0"
#define HEADER_Authorization "Authorization: Bearer J3ebj2iT\r\n"
#define API_UPLOAD_FILE "/api/upload"
#define API_UPDATE_NOTE "/api/update"
#define API_EXECUTE_COMMAND "/api/commands"
#define API_INSERT_NOTE "/api/insert"
#define HTTP_BOUNDARY "----WebKitFormBoundaryTjlkgpnCWY9MNBrA"

#define MIME_TYPE_JSON "application/json"


char *upload_image(const char *filepath);

char *execute_command(const char *cmd);

char *insert_note(const char *json_body);

char *update_note(const char *json_body);

#endif