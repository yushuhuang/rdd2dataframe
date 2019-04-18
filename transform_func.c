#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include "transform.h"

FILE *yyin;

char *sql;

ast_node *program(int low, int high, ast_node *mapOp)
{
    program_node *program_node = malloc(sizeof(struct program_node));
    program_node->node_type = 's';
    program_node->low = low;
    program_node->high = high;
    program_node->mapOp = mapOp;
    return (ast_node *)program_node;
}

ast_node *ast(int node_type, ast_node *l, ast_node *r)
{
    ast_node *ast_node = malloc(sizeof(struct ast_node));
    ast_node->node_type = node_type;
    ast_node->l = l;
    ast_node->r = r;
    return ast_node;
}

ast_node *assignment(char *ID, ast_node *r)
{
    assignment_node *assignment_node = malloc(sizeof(struct assignment_node));
    assignment_node->node_type = 'A';
    assignment_node->ID = ID;
    assignment_node->r = r;
    return (ast_node *)assignment_node;
}

ast_node *udf(char *ID, ast_node *r)
{
    udf_node *udf_node = malloc(sizeof(struct udf_node));
    udf_node->node_type = 'U';
    udf_node->ID = ID;
    udf_node->r = r;
    return (ast_node *)udf_node;
}

ast_node *dotID(char *l, char *r)
{
    dotID_node *dotID_node = malloc(sizeof(struct dotID_node));
    dotID_node->node_type = 'd';
    dotID_node->l = l;
    dotID_node->r = r;
    return (ast_node *)dotID_node;
}

ast_node *op(int op)
{
    operator_node *operator_node = malloc(sizeof(struct operator_node));
    operator_node->node_type = 'o';
    operator_node->op = op;
    return (ast_node *)operator_node;
}

ast_node *operation(ast_node *l, ast_node *op, ast_node *r)
{
    operation_node *operation_node = malloc(sizeof(struct operation_node));
    operation_node->node_type = 'O';
    operation_node->l = l;
    operation_node->op = op;
    operation_node->r = r;
    return (ast_node *)operation_node;
}

ast_node *comparator(comparatorType comp)
{
    comparator_node *comparator_node = malloc(sizeof(struct comparator_node));
    comparator_node->node_type = 'c';
    comparator_node->comp = comp;
    return (ast_node *)comparator_node;
}

ast_node *comparison(ast_node *l, ast_node *comp, ast_node *r)
{
    comparison_node *comparison_node = malloc(sizeof(struct comparison_node));
    comparison_node->node_type = 'C';
    comparison_node->l = l;
    comparison_node->comp = comp;
    comparison_node->r = r;
    return (ast_node *)comparison_node;
}

ast_node *ifstmt(ast_node *cond, ast_node *thenpart, ast_node *elsepart)
{
    if_node *if_node = malloc(sizeof(struct if_node));
    if_node->node_type = 'I';
    if_node->cond = cond;
    if_node->thenpart = thenpart;
    if_node->elsepart = elsepart;
    return (ast_node *)if_node;
}

ast_node *num(int n)
{
    num_node *num_node = malloc(sizeof(struct num_node));
    num_node->node_type = 'n';
    num_node->n = n;
    return (ast_node *)num_node;
}

ast_node *id(char *id)
{
    id_node *id_node = malloc(sizeof(struct id_node));
    id_node->node_type = 'i';
    id_node->id = id;
    return (ast_node *)id_node;
}

void free_ast_tree(ast_node *root)
{
    if (!root)
        return;

    switch (root->node_type)
    {
        /* two sub trees*/
    case 'T':
    case 'S':
    case 'M':
        free_ast_tree(root->l);
        free_ast_tree(root->r);
        break;
        /* one sub tree*/
    case 's':
    {
        program_node *node = (program_node *)root;
        free_ast_tree(node->mapOp);
    }
    break;
    /* no sub trees*/
    case 'd':
    case 'o':
    case 'c':
    case 'n':
    case 'i':
        break;
    /* ad-hoc */
    case 'U':
    case 'A':
    {
        assignment_node *node = (assignment_node *)root;
        free_ast_tree(node->r);
    }
    break;
    case 'O':
    {
        operation_node *node = (operation_node *)root;
        free_ast_tree(node->l);
        free_ast_tree(node->op);
        free_ast_tree(node->r);
    }
    break;
    case 'C':
    {
        comparison_node *node = (comparison_node *)root;
        free_ast_tree(node->l);
        free_ast_tree(node->comp);
        free_ast_tree(node->r);
    }
    break;
    case 'I':
    {
        if_node *node = (if_node *)root;
        free_ast_tree(node->cond);
        free_ast_tree(node->thenpart);
        free_ast_tree(node->elsepart);
    }
    }

    free(root);
}

void walk(ast_node *root, int level)
{
    printf("%*s", 2 * level, "");
    level++;
    if (!root)
        return;
    switch (root->node_type)
    {
    case 'T':
        printf("tuple\n");
        walk(root->l, level);
        walk(root->r, level);
        break;
    case 'S':
        printf("seq\n");
        walk(root->l, level);
        walk(root->r, level);
        break;
    case 'M':
        printf("map\n");
        walk(root->l, level);
        walk(root->r, level);
        break;
    case 's':
    {
        program_node *node = (program_node *)root;
        printf("sc.range(%d,%d)\n", node->low, node->high);
        printf("start\n");
        walk(node->mapOp, level);
    }
    break;
    case 'A':
    {
        assignment_node *node = (assignment_node *)root;
        printf("%s\n", node->ID);
        walk(node->r, level);
    }
    break;
    case 'U':
    {
        assignment_node *node = (assignment_node *)root;
        printf("%s\n", node->ID);
        walk(node->r, level);
    }
    break;
    case 'n':
    {
        num_node *node = (num_node *)root;
        printf("%d\n", node->n);
    }
    break;
    case 'i':
    {
        id_node *node = (id_node *)root;
        printf("%s\n", node->id);
    }
    break;
    case 'd':
    {
        dotID_node *node = (dotID_node *)root;
        printf("%s.%s\n", node->l, node->r);
    }
    break;
    case 'O':
    {
        operation_node *node = (operation_node *)root;
        printf("operation\n");
        walk(node->l, level);
        walk(node->op, level);
        walk(node->r, level);
    }
    break;
    case 'I':
    {
        if_node *node = (if_node *)root;
        printf("if\n");
        walk(node->cond, level);
        walk(node->thenpart, level);
        walk(node->elsepart, level);
    }
    break;
    case 'C':
    {
        comparison_node *node = (comparison_node *)root;
        printf("comparison\n");
        walk(node->l, level);
        walk(node->comp, level);
        walk(node->r, level);
    }
    break;
    case 'o':
    {
        operator_node *node = (operator_node *)root;
        printf("%c\n", node->op);
    }
    break;
    case 'c':
    {
        comparator_node *node = (comparator_node *)root;
        switch (node->comp)
        {
        case eq:
            printf("==\n");
            break;
        case lt:
            printf("<\n");
            break;
        case gt:
            printf(">\n");
            break;
        case ne:
            printf("!=\n");
            break;
        case ge:
            printf(">=\n");
            break;
        case le:
            printf("<=\n");
            break;
        }
    }
    }
}

int tuples;

void transformUDF(ast_node *root, GHashTable *symbolTable)
{
    if (!root)
        return;
    switch (root->node_type)
    {
    // Tuple
    case 'T':
    {
        transformUDF(root->l, symbolTable);
        tuples += 1;
        char t[12];
        sprintf(t, " as _%d\",\"", tuples);
        strcat(sql, t);
        transformUDF(root->r, symbolTable);
    }
    break;
    // Sequence
    case 'S':
    {
        transformUDF(root->l, symbolTable);
        transformUDF(root->r, symbolTable);
    }
    break;
    // Assignment
    case 'A':
    {
        assignment_node *node = (assignment_node *)root;
        symType *sym;
        char new_id[3];
        sprintf(new_id, "_%d", g_hash_table_size(symbolTable) + 1);
        sym = malloc(sizeof(struct symType));
        sym->new_id = new_id;
        sym->rhs = node->r;
        g_hash_table_insert(symbolTable, node->ID, sym);
    }
    break;
    // id
    case 'i':
    {
        id_node *node = (id_node *)root;
        symType *sym = g_hash_table_lookup(symbolTable, node->id);
        if (!sym)
        {
            char new_id[3];
            sprintf(new_id, "_%d", g_hash_table_size(symbolTable) + 1);
            sym = malloc(sizeof(struct symType));
            sym->new_id = new_id;
            sym->rhs = NULL;
            g_hash_table_insert(symbolTable, node->id, sym);
        }
        if (sym->rhs)
        {
            strcat(sql, "(");
            transformUDF(sym->rhs, symbolTable);
            strcat(sql, ")");
        }
        else
        {
            strcat(sql, sym->new_id);
        }
    }
    break;
    // dotID
    case 'd':
    {
        dotID_node *node = (dotID_node *)root;
        strcat(sql, node->r);
    }
    break;
    // Operation
    case 'O':
    {
        operation_node *node = (operation_node *)root;
        transformUDF(node->l, symbolTable);
        transformUDF(node->op, symbolTable);
        transformUDF(node->r, symbolTable);
    }
    break;
    // IF
    case 'I':
    {
        if_node *node = (if_node *)root;
        strcat(sql, "if(");
        transformUDF(node->cond, symbolTable);
        strcat(sql, ",");
        transformUDF(node->thenpart, symbolTable);
        strcat(sql, ",");
        transformUDF(node->elsepart, symbolTable);
        strcat(sql, ")");
    }
    break;
    // comparison
    case 'C':
    {
        comparison_node *node = (comparison_node *)root;
        transformUDF(node->l, symbolTable);
        transformUDF(node->comp, symbolTable);
        transformUDF(node->r, symbolTable);
    }
    break;
    // opeartor
    case 'o':
    {
        char sop[2];
        operator_node *node = (operator_node *)root;
        sprintf(sop, "%c", node->op);
        strcat(sql, sop);
    }
    break;
    // number
    case 'n':
    {
        char snum[10];
        num_node *node = (num_node *)root;
        sprintf(snum, "%d", node->n);
        strcat(sql, snum);
    }
    break;
    // comparator
    case 'c':
    {
        comparator_node *node = (comparator_node *)root;
        switch (node->comp)
        {
        case eq:
            strcat(sql, "==");
            break;
        case lt:
            strcat(sql, "<");
            break;
        case gt:
            strcat(sql, ">");
            break;
        case ne:
            strcat(sql, "!=");
            break;
        case ge:
            strcat(sql, ">=");
            break;
        case le:
            strcat(sql, "<=");
            break;
        }
    }
    }
}

void trans(ast_node *root)
{
    if (!root)
        return;
    switch (root->node_type)
    {
    // start
    case 's':
    {
        program_node *node = (program_node *)root;
        printf("spark.range(%d,%d).selectExpr(\"id as _1\")", node->low, node->high);
        trans(node->mapOp);
        printf(".collect()");
    }
    break;
    // mapops
    case 'M':
        trans(root->l);
        printf(".selectExpr(");
        trans(root->r);
        printf(")");
        break;
    // UDF
    case 'U':
    {
        udf_node *node = (udf_node *)root;
        GHashTable *symbolTable = g_hash_table_new(g_str_hash, g_str_equal);
        /* insert ID */
        char new_id[10];
        sprintf(new_id, "_%d", g_hash_table_size(symbolTable) + 1);
        symType *sym = malloc(sizeof(struct symType));
        sym->new_id = new_id;
        sym->rhs = NULL;
        g_hash_table_insert(symbolTable, node->ID, sym);
        sql = malloc(500);
        tuples = 0;
        transformUDF(node->r, symbolTable);
        printf("\"%s as _%d\"", sql, tuples + 1);
        free(sql);
        g_hash_table_destroy(symbolTable);
    }
    }
}

int main(int argc, char **argv)
{
    if (argc > 0)
        yyin = fopen(argv[1], "r");
    else
        yyin = stdin;
    return yyparse();
}

void yyerror(char const *s)
{
    fprintf(stderr, "%s\n", s);
}