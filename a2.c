/*============================================================================*
 *                              FILE: a2.c                                     *
 *                Skeleton Code: COMP10002 Assignment 2 2026                   *
 *              Written by: Kacie Beckett and Dr Hanzhi Wang                   *
 *                          LOST ITEM REGISTRY                                 *
 *          Edited by: [PLEASE ADD YOUR FULL NAME AND STUDENT ID HERE]         *
 *============================================================================*/
/*==========================================================*
 *                  PREPROCESSOR DIRECTIVES                  *
 *==========================================================*/
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LEN 256
#define COMMAND_LIST_DELIM ";"
#define COMMAND_DELIM " "

#define ITEM_ID_IDX 0
#define ITEM_OWNER_IDX (ITEM_ID_IDX + 1)
#define ITEM_DESCRIPTION_IDX (ITEM_OWNER_IDX + 1)
#define ITEM_LOCATION_IDX (ITEM_DESCRIPTION_IDX + 1)
#define ITEM_STATUS_IDX (ITEM_LOCATION_IDX + 1)
#define NUM_ITEM_PROPERTIES (ITEM_STATUS_IDX + 1)

#define ITEM_UID_STR "UID"
#define ITEM_OWNER_STR "OWNER"
#define ITEM_DESCRIPTION_STR "DESCRIPTION"
#define ITEM_LOCATION_STR "LOCATION"
#define ITEM_STATUS_STR "STATUS"
#define ITEM_STATUS_FOUND_STR "FOUND"
#define ITEM_STATUS_LOST_STR "LOST"

#define QUERY_FOUND_STR "Query for '%s' found the following matches:\n"
#define QUERY_FAIL_STR "Query for '%s' found no matches.\n"
#define ADD_SUCCESS_STR "Added item: "
#define ADD_FAIL_STR "Item with UID %d not added as it already exists\n"
#define STATS_LINE_STR "Stats for %s BST:\n"
#define STATS_TOTAL_ITEMS "> Total items: %d\n"
#define STATS_LOST_ITEMS "> Lost items: %d\n"
#define STATS_UNIQUE_PROPERTIES "> Unique Properties: %d\n"
#define STATS_TREE_HEIGHT "> Tree height: %d\n"
#define UPDATE_SUCCESS_STR "Updated item with UID %d to: "
#define UPDATE_FAILURE_STR "Item with UID %d not updated as it was not found\n"
#define DELETE_SUCCESS_STR "Deleted item: "
#define DELETE_FAILURE_STR "Item with UID %d not deleted as it was not found\n"
#define ITEM_LINE_FORMAT "%d | %s | %s | %s | %s\n"
#define ITEM_LIST_PREFIX "> "
#define PRINT_STR "Printing sorted by %s:\n"

/*==========================================================*
 *                      TYPE DEFINITIONS                     *
 *==========================================================*/

typedef enum {
    STATUS_LOST,
    STATUS_FOUND
} status_t;

typedef struct {
    int uid;
    char *owner;
    char *description;
    char *location;
    status_t status;
} item_t;

typedef struct bst_node bst_node_t;
struct bst_node {
    item_t *item;
    bst_node_t *left_arr[NUM_ITEM_PROPERTIES];
    bst_node_t *right_arr[NUM_ITEM_PROPERTIES];
};

typedef struct {
    int bst_node_count;
    int (*item_cmp_functions[NUM_ITEM_PROPERTIES])(item_t *, item_t *);
    bst_node_t *bst_root_arr[NUM_ITEM_PROPERTIES];
} registry_t;

/*==========================================================*
 *                   FUNCTION PROTOTYPES                     *
 *==========================================================*/

/* Command Handlers */
void registry_command_add(registry_t *reg, char *command_str);
void registry_command_query(registry_t *reg, char *command_str);
void registry_command_update(registry_t *reg, char *command_str);
void registry_command_print(registry_t *reg, char *command_str);
void registry_command_stats(registry_t *reg, char *command_str);
void registry_command_delete(registry_t *reg, char *id_txt);

void bst_find_in_order_successor_and_parent(bst_node_t *node,
                                            bst_node_t **successor,
                                            bst_node_t ***successor_parent_ptr,
                                            int attribute_index);
void bst_find_uid_match_and_parent(registry_t *reg, item_t *item,
    bst_node_t **node, bst_node_t ***parent_ptr, int attribute_index);
void bst_remove_links(bst_node_t **parent_ptr, bst_node_t *node,
    int attribute_index);
bst_node_t *bst_pop(registry_t *reg, int uid);

/* Comparison Functions */
int item_cmp_uid(item_t *new, item_t *existing);
int item_cmp_owner(item_t *new, item_t *existing);
int item_cmp_name(item_t *new, item_t *existing);
int item_cmp_location(item_t *new, item_t *existing);
int item_cmp_status(item_t *new, item_t *existing);

/* Memory Management */
registry_t *create_registry(
    int (*item_cmp_functions[NUM_ITEM_PROPERTIES])(item_t *, item_t *));
void free_registry(registry_t *reg);
void free_tree(bst_node_t *root);
bst_node_t *create_bst_node(item_t *item);
void free_bst_node(bst_node_t *bst_node);
item_t *create_item(int uid, char *owner, char *name, char *location,
                    status_t status);
void free_item(item_t *item);

/* General Helpers */
void process_line(registry_t *reg, char *line);
status_t parse_status(const char *text);
const char *status_to_string(status_t s);
int attribute_string_to_attribute_index(char *command);
void trim_newline(char *s);
void print_item_line(const item_t *item);
char *duplicate_string(const char *str, int str_len);
char *tokenize_string(const char *str, size_t *token_start_index, char *delim);
void print_in_order(bst_node_t *root, int attribute_index);

/*==========================================================*
 *                        MAIN LOGIC                         *
 *==========================================================*/

int main(void) {
    int (*item_cmp_functions[NUM_ITEM_PROPERTIES])(item_t *, item_t *) = {
        item_cmp_uid, item_cmp_owner, item_cmp_name, item_cmp_location,
        item_cmp_status};
    registry_t *reg = create_registry(item_cmp_functions);

    char line[MAX_LINE_LEN + 1];

    while (fgets(line, sizeof(line), stdin) != NULL) {
        // trim the newline from the line
        line[strcspn(line, "\n")] = '\0';
        if (strncmp(line, "==== STAGE", 10) == 0) {
            printf("%s\n", line);
            continue;
        }
        if (strcmp(line, "QUIT") == 0) {
            printf("QUITTING\n");
            break;
        }
        process_line(reg, line);
    }
    free_registry(reg);
    return 0;
}

/* Call the relevant command handler function for a input command

TODO: Uncomment the relevant command handler function
when it is implemented. (Commented out to start so it compiles)
*/
void process_line(registry_t *reg, char *line) {
    if (strncmp(line, "ADD ", 4) == 0) {
        registry_command_add(reg, line + 4);
    } else if (strncmp(line, "QUERY ", 6) == 0) {
        // registry_command_query(reg, line + 6);
    } else if (strncmp(line, "UPDATE ", 7) == 0) {
        // registry_command_update(reg, line + 7);
    } else if (strncmp(line, "PRINT ", 6) == 0) {
        registry_command_print(reg, line + 6);
    } else if (strncmp(line, "STATS ", 6) == 0) {
        // registry_command_stats(reg, line + 6);
    } else if (strncmp(line, "DELETE ", 7) == 0) {
        // registry_command_delete(reg, line + 7);
    } else {
        printf("Invalid Line: '%s'\n", line);
        assert(0);
    }
}

/*==========================================================*
 *                 FUNCTIONS TO IMPLEMENT                    *
 *==========================================================*/

/*====================== STAGE  0 ======================== */
/* a BST (binary search tree) node contains 
        (1) A pointer to an item
        (2) Left & Right child pointers 

    malloc(...) creates memory that persists after a function is ends
    e.g. item_t *item = malloc(sizeof(item_t)); => give me enough memory to store one item "item_t"
    AND return a pointer to it. 
    
    Arrays of pointers:
    bst_node_t *left_arr[NUM_ITEM_PROPERTIES] => is an array of POINTERS. NOT a pointer to an array*/

bst_node_t *create_bst_node(item_t *item) {
    /* TODO: Allocate space on the heap for a node and return a pointer to it */
    bst_node_t *node = malloc(sizeof(*node)); //
    assert(node != NULL);

    // Attach item
    node->item = item; // NOT COPYING! Just storing pointer to item

    // Set all children to NULL
    for (int i = 0; i < NUM_ITEM_PROPERTIES; i++){
        node->left_arr[i] = NULL;
        node->right_arr[i] = NULL;
    }

    // Finally return node
    return node;
}

void free_bst_node(bst_node_t *bst_node) {
    /* Because we want to free node (which has item in it which owns heap strings) we first free item using our free_item function */
    free_item(bst_node->item);
    free(bst_node);
}

item_t *create_item(int uid, char *owner, char *description, char *location,
                    status_t status) {
    // We need the memory for our item_t first
    item_t *item = malloc(sizeof(item_t)); // Now item points to an empty struct in heap memory

    // Check malloc worked s.t. if not (i.e. malloc() return NULL) segmentation fault (and other problems) occur
    assert(item != NULL);

    /* Remember here, item points to the address of the struct. To access the struct we must follow the pointer and THEN access the field
    (*item).uid but item->uid does this exact thing and is more read-able and write-able 
    thus, for non-string/"normal" values*/
    item->uid = uid;
    item->status = status; 
    
    /* For string values we remind ourselves that strings are just pointers to memory => og. memory may not stay valid. So we duplicate them! */
    item->owner = duplicate_string(owner, strlen(owner));
    item->description = duplicate_string(description, strlen(description));
    item->location = duplicate_string(location, strlen(location));

    // Finally return our item
    return item;
}

void free_item(item_t *item) {
    /* Note: the struct itself (item) is heap memory, and each string is also seperate heap memory bcs. duplicate_string used malloc() 
    Thus, we have had 4 allocations
    (1) item
    (2) owner
    (3) description
    (4) location 
    So we will free everything owned by item starting by freeing internal heap memory first... then freeing outer struct last*/
    free(item->owner);
    free(item->description);
    free(item->location);
    free(item);
}

/*====================== STAGE  1 ======================== */
/* Visually our registry looks like this (leaving out a few item descriptors for simplicity)
           registry
          /   |   \
         /    |    \
      UID   OWNER  LOCATION 
      BST    BST      BST
    Each BST is sorted differently, BUT they all contain the same items*/

void registry_command_add(registry_t *reg, char *command_str) {
    /*Goal of function:
    (1) Extract values from command_str
    (2) Create item
    (3) Create BST node
    (4) Print success*/

    char *properties[NUM_ITEM_PROPERTIES]; // Array of pointers to characters
    size_t curr_token_start_index = 0;
    int i = 0;

    /* The input format requires parsing 6 values
    What the loop does: loops through properties s.t.
    properties[0] = uid
    properties[1] = owner
    properties[2] = description... and so on*/
    char *tok;
    while (i < NUM_ITEM_PROPERTIES) {
        tok = tokenize_string(command_str, &curr_token_start_index,
                              COMMAND_LIST_DELIM);
        assert(tok != NULL);
        properties[i++] = tok;
    }

    int uid = atoi(properties[ITEM_ID_IDX]);
    free(properties[ITEM_ID_IDX]);
    status_t status = parse_status(properties[ITEM_STATUS_IDX]); // "LOST" -> STATUS_LOST
    free(properties[ITEM_STATUS_IDX]);

    char *owner = properties[ITEM_OWNER_IDX];
    char *description = properties[ITEM_DESCRIPTION_IDX];
    char *location = properties[ITEM_LOCATION_IDX];

    item_t *item = create_item(uid, owner, description, location, status);
    bst_node_t *node = create_bst_node(item);

    /* Memory is now s.t. (visually)
      node
        └── item
              ├── uid
              ├── owner
              ├── description
              └── ...
    */
    if (node != NULL) { 
        /*
        TODO:
        - Insert the newly created node into the BST for each attribute from
          0 to (NUM_ITEM_PROPERTIES - 1).
        - Print the ADD_SUCCESS_STR and item line if successful
        - Note you can access the comparison function with
          reg->item_cmp_functions[attribute_index]

        Note: You may wish to start by implementing your code for only the UID
              BST (which is index 0)
        */
        /* Empty tree case */
        /* Recalling:
        bst_node_t *bst_root_arr[NUM_ITEM_PROPERTIES]; is an array of ROOT POINTERS for the BSTs
        bst_root_arr[0] = root of UID BST
        bst_root_arr[1] = root of OWNER BST
        bst_root_arr[2] = root of DESCRIPTION BST
        bst_root_arr[3] = root of LOCATION BST
        bst_root_arr[4] = root of STATUS BST */
        int attribute_index;
        for (attribute_index = 0; attribute_index < NUM_ITEM_PROPERTIES; attribute_index++){
            // Case where root of BST doesnt exist
            if (reg->bst_root_arr[attribute_index] == NULL){ // attribute_index = 0 => looking at root of UID BST... etc
                reg->bst_root_arr[attribute_index] = node;
            } else {
                bst_node_t *curr = reg->bst_root_arr[attribute_index]; // Start traversal (comparison) at root of BST

                while (1) { // Break when we insert node into BST
                    // Create a value for our comparison for our Dupe / left / right logic
                    int cmp = reg->item_cmp_functions[attribute_index](item,curr->item);

                    /* Duplicate uid */
                    if ((cmp == 0) && (attribute_index == ITEM_ID_IDX)){ // UID equality means duped item, thus, we dont want it. But we can have cmp = 0 and not be a duped item (cmp func for owner per say = 0 is fine because same person might lose mult things)
                        printf(ADD_FAIL_STR, item->uid);
                        free_bst_node(node);
                        return;
                    }

                    /* Going left */
                    if (cmp < 0){

                        // if no left child exists, insert node here
                        if (curr->left_arr[attribute_index] == NULL){
                            curr->left_arr[attribute_index] = node;
                            break;
                        }

                        // Otherwise move left
                        curr = curr->left_arr[attribute_index];

                    } else /* Going right i.e. cmp > 0*/ {

                        // If no right child exists, insert node here
                        if (curr->right_arr[attribute_index] == NULL){
                            curr->right_arr[attribute_index] = node;
                            break;
                        }

                        // Otherwise move right 
                        curr = curr->right_arr[attribute_index];
                    }
                }
            }
        }

        // Successful insertion of node
        reg->bst_node_count++;
        printf(ADD_SUCCESS_STR);
        print_item_line(item);    
    } else {
        return;
    }
}

void registry_command_print(registry_t *reg, char *command_str) {
    size_t curr_token_start_index = 0;
    char *query_type =
        tokenize_string(command_str, &curr_token_start_index, COMMAND_DELIM);
    assert(query_type != NULL);
    int attribute_index = attribute_string_to_attribute_index(query_type);

     // Printing our heading
     printf(PRINT_STR, query_type);

     // Start from the root of queried BST
     bst_node_t *root_node = reg->bst_root_arr[attribute_index];

     print_in_order(root_node, attribute_index);
}

/*====================== STAGE  2 ======================== */

void registry_command_stats(registry_t *reg, char *command_str) {
    /* TODO: Print the total number of items in the BST, the number of unique
     * values of the given attribute type, and the height of the tree. In the
     * case of the status attribute, also print the number of lost items. */
}

/*====================== STAGE  3 ======================== */

void registry_command_query(registry_t *reg, char *command_str) {
    size_t curr_token_start_index = 0;
    char *query_type =
        tokenize_string(command_str, &curr_token_start_index, COMMAND_DELIM);
    assert(query_type != NULL);

    char *query_str =
        duplicate_string(command_str + curr_token_start_index,
                         strlen(command_str + curr_token_start_index));
    assert(query_str != NULL);

    int attribute_index = attribute_string_to_attribute_index(query_type);

    // Suppress compilation warning until function is implemented
    (void)attribute_index;

    /* TODO: Print all the records matching the query type using the
     * corresponding BST */
    //printf()
}

/*====================== STAGE  4 ======================== */

void bst_find_in_order_successor_and_parent(bst_node_t *node,
                                            bst_node_t **successor,
                                            bst_node_t ***successor_parent_ptr,
                                            int attribute_index) {
    /* TODO: Find the inorder successor and save it, aswell a pointer,
     to the parent pointer which links to the inorder successor.
     It should be true that for non NULL variables:
        `**successor_parent_ptr == *successor` at the end of your function
     */
}

void bst_find_uid_match_and_parent(registry_t *reg, item_t *item,
                                   bst_node_t **node, bst_node_t ***parent_ptr,
                                   int attribute_index) {
    /* TODO: Find a node in a given attribute BST and a pointer to the
    parent pointer which links to it.
     It should be true that for non NULL variables:
        `**parent_ptr == *node` at the end of your function
    */
}


void registry_command_delete(registry_t *reg, char *command_str) {
    int uid = atoi(command_str);
    bst_node_t *bst_node = bst_pop(reg, uid);

    if (bst_node != NULL) {
        printf(DELETE_SUCCESS_STR);
        print_item_line(bst_node->item);
        free_bst_node(bst_node);
        reg->bst_node_count--;
    } else {
        printf(DELETE_FAILURE_STR, uid);
    }
}

/*====================== STAGE  5 ======================== */

void registry_command_update(registry_t *reg, char *command_str) {
    /* TODO: Handle the update command so that attributes
    (except for UID) can be modified */
}

/*==========================================================*
 *             STAGE 4 Provided Functions                   *
 *==========================================================*/

/*
Remove the node corresponding to a specifc item by identifying it using
the unique identifier (uid), updating the links in all attribute
binary search trees before returning a pointer to the removed node.
*/
bst_node_t *bst_pop(registry_t *reg, int uid) {
    item_t id_item = {.uid = uid};
    bst_node_t *node;
    bst_node_t **parent_ptr;

    // Find the node by UID
    bst_find_uid_match_and_parent(reg, &id_item, &node, &parent_ptr,
                                  ITEM_ID_IDX);

    if (node != NULL) {
        item_t *item = node->item;
        assert(parent_ptr != NULL);
        bst_remove_links(parent_ptr, node, ITEM_ID_IDX);
        for (int i = 1; i < NUM_ITEM_PROPERTIES; i++) {
            // Find the pointer from the parent in the corresponding
            // left or right array, by searching for the node
            // by UID.
            bst_find_uid_match_and_parent(reg, item, &node, &parent_ptr, i);
            assert(node != NULL && parent_ptr != NULL);
            bst_remove_links(parent_ptr, node, i);
        }
        reg->bst_node_count--;
    }
    return node;
}

/*
Delete the links for the given node within the attribute binary search tree
for a given attribute. Notably this does *not* free the memory associated
with the node as the links must be removed from all attribute
binary search trees first, to avoid dangling pointers.
*/
void bst_remove_links(bst_node_t **parent_ptr, bst_node_t *node,
                      int attribute_index) {
    assert(*parent_ptr == node);
    int node_has_left_child = node->left_arr[attribute_index] != NULL;
    int node_has_right_child = node->right_arr[attribute_index] != NULL;

    if (!node_has_left_child && !node_has_right_child) {
        // CASE: Leaf node which can be removed directly
        *parent_ptr = NULL;

    } else if (!node_has_left_child || !node_has_right_child) {
        // CASE: Node with only one child node
        *parent_ptr = node_has_left_child ? node->left_arr[attribute_index]
                                          : node->right_arr[attribute_index];
        node->left_arr[attribute_index] = NULL;
        node->right_arr[attribute_index] = NULL;
    } else {
        // CASE: Node with two child nodes
        // To remove in this case, we should swap with the in order successor
        // and then call bst_delete on the node again as node will
        // be guaranteed to be a base case of having either 0 or 1 children.
        bst_node_t **successor_parent_ptr = NULL;
        bst_node_t *successor = NULL;
        bst_find_in_order_successor_and_parent(
            node, &successor, &successor_parent_ptr, attribute_index);

        assert(successor != NULL);

        bst_node_t **tmp = parent_ptr;
        parent_ptr = successor_parent_ptr;
        successor_parent_ptr = tmp;

        bst_node_t *left_tmp = successor->left_arr[attribute_index];
        bst_node_t *right_tmp = successor->right_arr[attribute_index];

        // SPECIAL CASE: Successor is direct child of the node to delete
        // so swapping the parent pointers around will lead to broken link
        // as the relative position changes.
        if (successor == node->right_arr[attribute_index]) {
            parent_ptr = &(successor->right_arr[attribute_index]);
        }

        // Swap the corresponding left and right pointers
        successor->left_arr[attribute_index] = node->left_arr[attribute_index];
        successor->right_arr[attribute_index] =
            node->right_arr[attribute_index];
        node->left_arr[attribute_index] = left_tmp;
        node->right_arr[attribute_index] = right_tmp;

        // Set the parents to point to the new nodes
        *parent_ptr = node;
        *successor_parent_ptr = successor;

        // Recursively call delete on the node, as it is now
        // guaranteed to be a base case of 0 or 1 children
        bst_remove_links(parent_ptr, node, attribute_index);
    }
}

/*==========================================================*
 *                    COMPARISON FUNCTIONS                   *
 *==========================================================*/
int item_cmp_owner(item_t *new, item_t *existing) {
    return strcmp(new->owner, existing->owner);
}

int item_cmp_location(item_t *new, item_t *existing) {
    return strcmp(new->location, existing->location);
}

int item_cmp_name(item_t *new, item_t *existing) {
    return strcmp(new->description, existing->description);
}

int item_cmp_status(item_t *new, item_t *existing) {
    return (int)new->status - (int)existing->status;
}

int item_cmp_uid(item_t *new, item_t *existing) {
    return new->uid - existing->uid;
}

/*==========================================================*
 *                     HELPER FUNCTIONS                      *
 *==========================================================*/

/* Initalise space for an empty registry on the heap */
registry_t *create_registry(
    int (*item_cmp_functions[NUM_ITEM_PROPERTIES])(item_t *, item_t *)) {
    registry_t *reg = (registry_t *)malloc(sizeof(*reg));
    assert(reg != NULL);
    for (int i = 0; i < NUM_ITEM_PROPERTIES; i++) {
        reg->bst_root_arr[i] = NULL;
        reg->item_cmp_functions[i] = item_cmp_functions[i];
    }
    reg->bst_node_count = 0;
    return reg;
}

/* Free heap allocated memory for the registry and all data in the linked
 * structure */
void free_registry(registry_t *reg) {
    assert(reg != NULL);
    free_tree(reg->bst_root_arr[0]);
    for (int i = 0; i < NUM_ITEM_PROPERTIES; i++) {
        reg->bst_root_arr[i] = NULL;
    }

    reg->bst_node_count = 0;
    free(reg);
}

/* Recursively free memory for nodes in the binary search tree */
void free_tree(bst_node_t *node) {
    if (node == NULL) {
        return;
    }

    // Should only free from one of the linked trees
    free_tree(node->left_arr[ITEM_ID_IDX]);
    free_tree(node->right_arr[ITEM_ID_IDX]);
    for (int i = 0; i < NUM_ITEM_PROPERTIES; i++) {
        node->left_arr[i] = NULL;
        node->right_arr[i] = NULL;
    }
    free_bst_node(node);
}

/* Convert a status string into a status state */
status_t parse_status(const char *text) {
    if (strcmp(text, ITEM_STATUS_FOUND_STR) == 0) {
        return STATUS_FOUND;
    }
    return STATUS_LOST;
}

/* Convert a status status stat back into a status string */
const char *status_to_string(status_t s) {
    return s == STATUS_FOUND ? ITEM_STATUS_FOUND_STR : ITEM_STATUS_LOST_STR;
}

/* Get the attribute index to access the linked BST or linkage arrays */
int attribute_string_to_attribute_index(char *command) {
    int attribute_index;

    if (strcmp(command, ITEM_UID_STR) == 0) {
        attribute_index = ITEM_ID_IDX;
    } else if (strcmp(command, ITEM_OWNER_STR) == 0) {
        attribute_index = ITEM_OWNER_IDX;
    } else if (strcmp(command, ITEM_DESCRIPTION_STR) == 0) {
        attribute_index = ITEM_DESCRIPTION_IDX;
    } else if (strcmp(command, ITEM_LOCATION_STR) == 0) {
        attribute_index = ITEM_LOCATION_IDX;
    } else if (strcmp(command, ITEM_STATUS_STR) == 0) {
        attribute_index = ITEM_STATUS_IDX;
    } else {
        assert(0);
    }
    return attribute_index;
}

/* Print the attributes associated with the item following the output format */
void print_item_line(const item_t *item) {
    printf(ITEM_LINE_FORMAT, item->uid, item->owner, item->description,
           item->location, status_to_string(item->status));
}


/* Make a copy of a NUL terminated string on the heap */
char *duplicate_string(const char *str, int str_len) {
    char *copy = (char *)malloc(sizeof(*copy) * (str_len + 1));
    assert(copy != NULL);
    memcpy(copy, str, str_len);
    copy[str_len] = '\0';
    return copy;
}

/*
Safe implementation for tokenizing a string. Unlike strtok from <stdlib.h>
this does not modify the original string. Returns new NUL terminated heap
allocated string, and the index of the character ahead of the delimiter
for subsequent calls. */
char *tokenize_string(const char *str, size_t *token_start_index, char *delim) {
    assert(strlen(delim) == 1);
    assert(*token_start_index < strlen(str));

    size_t token_len = strcspn(str + *token_start_index, delim);

    /* Allocate space for the string plus a NUL byte */
    char *copy = duplicate_string(str + *token_start_index, token_len);

    /* adjust the start index to get the next token */
    *token_start_index += token_len + 1;
    return copy;
}

void print_in_order(bst_node_t *root, int attribute_index){
    if (root == NULL){
        return;
    }
    // Print left subtree first using recursion FIRST!
    print_in_order(root->left_arr[attribute_index], attribute_index);

    // Print current node
    print_item_line(root->item);

    // Print right subtree using recursion AFTER!
    print_in_order(root->right_arr[attribute_index], attribute_index);
}