#ifndef __BUILDER_H__
#define __BUILDER_H__

#include <ast.h>

#include <stdbool.h>

typedef enum builder_priority_group {
    BUILDER_PG_NB = 0, /* Не собираются */
    BUILDER_PG_L, /* Собираются в последнюю очередь */
    BUILDER_PG_12, /* Приоритет группы 12 */
    BUILDER_PG_11, /* Приоритет группы 11 */
    BUILDER_PG_10, /* Приоритет группы 10 */
    BUILDER_PG_9, /* Приоритет группы 9 */
    BUILDER_PG_8, /* Приоритет группы 8 */
    BUILDER_PG_7, /* Приоритет группы 7 */
    BUILDER_PG_6, /* Приоритет группы 6 */
    BUILDER_PG_5, /* Приоритет группы 5 */
    BUILDER_PG_3, /* Приоритет группы 3 */
    BUILDER_PG_H, /* Собираются в первую очередь */
    BUILDER_PG_A /* Аннотации собираются раньше всего */
} builder_priority_group_t;

bool builder_build_body_cycle(ast_body_t* body);
bool builder_build_body(ast_body_t* body, uint8_t priority);
bool builder_build_expression(ast_body_t* body, ast_expr_t* last);

uint8_t builder_priority(ast_expr_t* expression);

#endif /* __BUILDER_H__ */