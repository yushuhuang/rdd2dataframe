int yylex(void);
int yyparse(void);
void yyerror(const char *s);
#define YYERROR_VERBOSE

typedef enum comparatorType
{
    eq,
    lt,
    gt,
    ne,
    ge,
    le
} comparatorType;

typedef struct ast_node
{
    int node_type;
    struct ast_node *l;
    struct ast_node *r;
} ast_node;

typedef struct symType
{
    char *new_id;
    ast_node *rhs;
} symType;

typedef struct program_node
{
    int node_type;
    int low;
    int high;
    struct ast_node *mapOp;
} program_node;

typedef struct assignment_node
{
    int node_type;
    char *ID;
    ast_node *r;
} assignment_node;

typedef struct udf_node
{
    int node_type;
    char *ID;
    ast_node *r;
} udf_node;

typedef struct dotID_node
{
    int node_type;
    char *l;
    char *r;
} dotID_node;

typedef struct operator_node
{
    int node_type;
    int op;
} operator_node;

typedef struct operation_node
{
    int node_type;
    struct ast_node *l;
    struct ast_node *op;
    struct ast_node *r;
} operation_node;

typedef struct comparator_node
{
    int node_type;
    comparatorType comp;
} comparator_node;

typedef struct comparison_node
{
    int node_type;
    struct ast_node *l;
    struct ast_node *comp;
    struct ast_node *r;
} comparison_node;

typedef struct if_node
{
    int node_type;
    struct ast_node *cond;
    struct ast_node *thenpart;
    struct ast_node *elsepart;
} if_node;

typedef struct num_node
{
    int node_type;
    int n;
} num_node;

typedef struct id_node
{
    int node_type;
    char *id;
} id_node;

ast_node *program(int low, int high, ast_node *mapOp);
ast_node *ast(int node_type, ast_node *l, ast_node *r);
ast_node *assignment(char *ID, ast_node *r);
ast_node *udf(char *ID, ast_node *r);
ast_node *dotID(char *l, char *r);
ast_node *op(int op);
ast_node *operation(ast_node *l, ast_node *op, ast_node *r);
ast_node *comparator(comparatorType comp);
ast_node *comparison(ast_node *l, ast_node *comp, ast_node *r);
ast_node *ifstmt(ast_node *cond, ast_node *thenpart, ast_node *elsepart);
ast_node *num(int n);
ast_node *id(char *id);

/* debug print out ast */
void walk(ast_node *root, int level);

void trans(ast_node *root);

void free_ast_tree(ast_node *tree);