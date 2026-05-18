/*============================================================================*
 *                              FILE: a2.c                                     *
 *                Skeleton Code: COMP10002 Assignment 2 2026                   *
 *              Written by: Kacie Beckett and Dr Hanzhi Wang                   *
 *                          LOST ITEM REGISTRY                                 *
 *          Edited by: Jude Docherty Scott - 1616474         *
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
#define STATS_UNIQUE_VALUES "> Unique values: %d\n"
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
int count_total_nodes(bst_node_t *root, int attribute_index);
void count_unique_vals(bst_node_t *root, int attribute_index, item_t **prev_item, int *cnt);
int count_tree_height(bst_node_t *root, int attribute_index);
void query_bst(bst_node_t *root, int attribute_index, char *query_str);
void find_uid_helper(bst_node_t **curr_ptr, item_t *item, bst_node_t **node, bst_node_t ***parent_ptr, int attribute_index);
void registry_command_reinsert(registry_t *reg, char *command_str);

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

/* Call the relevant command handler function for a input command */
void process_line(registry_t *reg, char *line) {
    if (strncmp(line, "ADD ", 4) == 0) {
        registry_command_add(reg, line + 4);
    } else if (strncmp(line, "QUERY ", 6) == 0) {
        registry_command_query(reg, line + 6);
    } else if (strncmp(line, "UPDATE ", 7) == 0) {
        registry_command_update(reg, line + 7);
    } else if (strncmp(line, "PRINT ", 6) == 0) {
        registry_command_print(reg, line + 6);
    } else if (strncmp(line, "STATS ", 6) == 0) {
        registry_command_stats(reg, line + 6);
    } else if (strncmp(line, "DELETE ", 7) == 0) {
        registry_command_delete(reg, line + 7);
    } else {
        printf("Invalid Line: '%s'\n", line);
        assert(0);
    }
}

/*==========================================================*
 *                 FUNCTIONS TO IMPLEMENT                    *
 *==========================================================*/

/*====================== STAGE  0 ======================== */
/* =============== General conceptual notes ===============
 a BST (binary search tree) node contains 
        (1) A pointer to an item
        (2) Left & Right child pointers 

    malloc(...) creates memory that persists after a function ends
    e.g. item_t *item = malloc(sizeof(item_t)); => give me enough memory to store one item "item_t"
    AND return a pointer to it. 
    
    Arrays of pointers:
    bst_node_t *left_arr[NUM_ITEM_PROPERTIES] => is an array of POINTERS. NOT a pointer to an array*/

bst_node_t *create_bst_node(item_t *item) {
    /* =============== Goal ===============
    Allocate space on the heap for a BST node and return a pointer to it:
    (1) Allocate space on the heap for the node
    (2) Attach item to node
    (3) Set all children to NULL
    (4) Return pointer to node */

    /* =============== Step 1 =============== */
    bst_node_t *node = malloc(sizeof(*node));
    assert(node != NULL);

    /* =============== Step 2 =============== */
    node->item = item; // NOT COPYING! Just storing pointer to item

    /* =============== Step 3 =============== */
    for (int i = 0; i < NUM_ITEM_PROPERTIES; i++){
        node->left_arr[i] = NULL;
        node->right_arr[i] = NULL;
    }

    /* =============== Step 4 =============== */
    return node;
}

void free_bst_node(bst_node_t *bst_node) {
    /* =============== Goal ===============
    Free a node - Implementation:
    (1) Node contains an item - item has heap strings => free item first
    (2) Free node */
    /* =============== Step 1 =============== */
    free_item(bst_node->item);
    /* =============== Step 2 =============== */
    free(bst_node);
}

item_t *create_item(int uid, char *owner, char *description, char *location,
                    status_t status) {
    /* =============== Goal =============== 
    To create an item we must
    (1) Allocate heap memory for a new item_t struct
    (2) Copy / store the provided values (function inputs) into the struct
    (3) Return pointer to the newly created item */
    
    /* =============== Step 1 =============== */
    item_t *item = malloc(sizeof(item_t)); // Now item points to an empty struct in heap memory
    assert(item != NULL); // Check malloc worked - if NULL => segmentation fault (other problems)

    /* =============== Step 2 =============== */
    /* Remember here, item points to the address of the struct. To access the struct we must follow the pointer 
    and THEN access the field (*item).uid but item->uid does this exact thing and is more read-able and write-able 
    thus, for non-string / "normal" values */
    item->uid = uid;
    item->status = status; 
    
    /* For string values we remind ourselves that strings are just pointers to memory => 
    original memory may not stay valid. So we duplicate them! */
    item->owner = duplicate_string(owner, strlen(owner));
    item->description = duplicate_string(description, strlen(description));
    item->location = duplicate_string(location, strlen(location));
    
    /* =============== Step 3 =============== */
    return item;
}

void free_item(item_t *item) {
    /* =============== Goal =============== 
    Free item:
    (1) free struct string heap memory 
    (2) free the struct (item) heap memory */
    
    /* Note: the struct itself (item) is heap memory, and each string is also seperate heap memory 
    bcs. duplicate_string used malloc() thus, we have had 4 allocations
    (1) item
    (2) owner
    (3) description
    (4) location 
    So we will free everything owned by item starting by freeing internal heap memory first...
    then freeing outer struct last*/
    /* =============== Step 1 =============== */
    free(item->owner);
    free(item->description);
    free(item->location);
    /* =============== Step 2 =============== */
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
    /* =============== Goal =============== 
    (1) Extract (parse) values from command_str
    (2) Create item / Create BST node
    (3) Insert node into appropriate positon on BST
    (4) Print success */

    /* =============== Step 1 =============== */
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

    /* =============== Step 2 =============== */
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

    /* =============== Step 3 =============== */
    if (node != NULL) {
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

        /* =============== Step 4 =============== */
        // Successful insertion of node
        reg->bst_node_count++;
        printf(ADD_SUCCESS_STR);
        print_item_line(item);    
    } else {
        return;
    }
}

void registry_command_print(registry_t *reg, char *command_str) {
    /* =============== Goal =============== 
    Print all entries of queried BST:
    (1) Parse command string for which BST to print
    (2) Print standard heading
    (3) Print in-order BST */

    /* =============== Step 1 =============== */
    size_t curr_token_start_index = 0;
    char *query_type =
        tokenize_string(command_str, &curr_token_start_index, COMMAND_DELIM);
    assert(query_type != NULL);
    int attribute_index = attribute_string_to_attribute_index(query_type);

     /* =============== Step 2 =============== */
     printf(PRINT_STR, query_type);

     /* =============== Step 3 =============== */
     bst_node_t *root_node = reg->bst_root_arr[attribute_index];

     print_in_order(root_node, attribute_index);
}

/*====================== STAGE  2 ======================== */

void registry_command_stats(registry_t *reg, char *command_str) {
    /* =============== Goal =============== 
    (1) Extract whicj item property is the subject of the query
    (2) Print generic stats line
    (3) Print stats for selected BST*/
    /* =============== Step 1 =============== */
    size_t curr_token_start_index = 0;
    char * query_type = 
        tokenize_string(command_str, &curr_token_start_index, COMMAND_DELIM);
    assert(query_type != NULL);
    int attribute_index = attribute_string_to_attribute_index(query_type);

    /* =============== Step 2 =============== */
    printf(STATS_LINE_STR, query_type);

    /* =============== Step 3 =============== */
    bst_node_t *root = reg->bst_root_arr[attribute_index];
    // Case: Empty BST
    if (root == NULL){
        printf(STATS_TOTAL_ITEMS, 0);
        printf(STATS_UNIQUE_VALUES, 0);
        printf(STATS_TREE_HEIGHT, 0);
        return;
    }

    /* Calculating total items */
    int total_item_cnt;
    total_item_cnt = count_total_nodes(root, attribute_index);
    printf(STATS_TOTAL_ITEMS, total_item_cnt);

    /* Calculating unique items */
    int unique_item_cnt = 0;
    item_t *prev_item = NULL;
    count_unique_vals(root, attribute_index, &prev_item, &unique_item_cnt);
    printf(STATS_UNIQUE_VALUES, unique_item_cnt);

    /* Calculating tree height */
    int tree_height;
    tree_height = count_tree_height(root, attribute_index);
    printf(STATS_TREE_HEIGHT, tree_height);
}

/*====================== STAGE  3 ======================== */

void registry_command_query(registry_t *reg, char *command_str) {
    /* =============== Goal =============== 
    (1) Parse command string for queried item property
    (2) Find matches in BST order and print in ascending (as per usual) order */

    /* =============== Step 1 =============== */
    size_t curr_token_start_index = 0;
    char *query_type =
        tokenize_string(command_str, &curr_token_start_index, COMMAND_DELIM);
    assert(query_type != NULL);

    char *query_str =
        duplicate_string(command_str + curr_token_start_index,
                         strlen(command_str + curr_token_start_index));
    assert(query_str != NULL);

    int attribute_index = attribute_string_to_attribute_index(query_type);
    
    /* =============== Step 2 =============== */
    bst_node_t *root = reg->bst_root_arr[attribute_index];
    query_bst(root, attribute_index, query_str);
}

/*====================== STAGE  4 ======================== */

void bst_find_in_order_successor_and_parent(bst_node_t *node,
                                            bst_node_t **successor,
                                            bst_node_t ***successor_parent_ptr,
                                            int attribute_index) {
    /* =============== Goal =============== 
    (1) Find inorder successor and save it, where:
    The inorder successor is the left-most node relative to the right child of the given node
    (2) Find pointer to the parent pointer, where:
    *successor_parent_ptr = &(parent->left_arr[attribute_index]) */

    // Null node check
    if (node == NULL){
        *successor = NULL;
        *successor_parent_ptr = NULL;
        return;
    }

    // Start from the right subtree (right-left-left...-left-NULL)
    bst_node_t *start = node->right_arr[attribute_index];
    bst_node_t *parent = node;
    bst_node_t *curr = start;

    // If no right subtree exists
    if (curr == NULL){
        *successor = NULL;
        *successor_parent_ptr = NULL;
        return;
    }

    // Traverse left until NULL
    while (curr->left_arr[attribute_index] != NULL){
        parent = curr;
        curr = curr->left_arr[attribute_index];
    }
    
    *successor = curr;
    *successor_parent_ptr = &(parent->left_arr[attribute_index]);
    
}

void bst_find_uid_match_and_parent(registry_t *reg, item_t *item,
                                   bst_node_t **node, bst_node_t ***parent_ptr,
                                   int attribute_index) {
    /* =============== Goal =============== 
    Find item node which contains item with matching UID - starting at root of BST */

    // Initialise *node == NULL as we want to initially pass it as NULL into helper function
    // => **parent_ptr == NULL
    *node = NULL;
    *parent_ptr = NULL;
    find_uid_helper(&(reg->bst_root_arr[attribute_index]), item, node, parent_ptr, attribute_index);
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
    /* =============== Goal =============== 
    (1) Pass command string:
        Acquire UID, item property and new property
    (2) Find the node using bst_find_uid_match_and_parent (STAGE 4)
    (3) Remove the node from all BSTs using bst_pop 
        We use bst_pop & not free_bst_node because we want to 
        (a) detach the node
        (b) modify the item value 
        (c) re-instert node into correct place on all BSTs
        Thus, we dont want to free the node (deallocate the memory)
    (4) Modify the item field:
        (a) Free the value initially there
        (b) Re-allocate heap memory to new value 
    (5) Reinsert node into ALL BSTs using slightly altered stage 1 function registry_command_reinsert */

    /* =============== Step 1 =============== */
    size_t curr_token_start_index = 0;
    char *uid_str =
    tokenize_string(command_str, &curr_token_start_index, COMMAND_DELIM);
    int uid = atoi(uid_str);
    free(uid_str); // Release heap memory

    /* Note:
    update_type = the type we are trying to update
    update_str = the value we are trying to update that type to (a str unless update_type = status) */
    char *update_type =
        tokenize_string(command_str, &curr_token_start_index, COMMAND_DELIM);
    assert(update_type != NULL);

    char *update_str =
        duplicate_string(command_str + curr_token_start_index,
                         strlen(command_str + curr_token_start_index));
    assert(update_str != NULL);
 
    int attribute_index = attribute_string_to_attribute_index(update_type);

    /* =============== Step 2 ===============  
    Recall: We pass item into bst_find_uid_match_and_parent - item is only used as a search key.*/
    item_t search_key_item;
    search_key_item.uid = uid; // Only UID is being used as the search key so otherwise NULL propertys are fine
    bst_node_t *node = NULL;
    bst_node_t **parent_ptr = &node;
    // ITEM_ID_IDX: searching solely by UID
    bst_find_uid_match_and_parent(reg, &search_key_item, &node, &parent_ptr, ITEM_ID_IDX); 

    /* =============== Step 3 =============== */
    bst_pop(reg, uid);

    /* =============== Step 4 =============== */
    switch(attribute_index){
        // Owner
        case ITEM_OWNER_IDX:
            free(node->item->owner);
            node->item->owner = duplicate_string(update_str, strlen(update_str));
        break;

        // Description
        case ITEM_DESCRIPTION_IDX:
            free(node->item->description);
            node->item->description = duplicate_string(update_str, strlen(update_str));
        break;

        // Location
        case ITEM_LOCATION_IDX:
            free(node->item->location);
            node->item->location = duplicate_string(update_str, strlen(update_str));
        break;

        // Status
        case ITEM_STATUS_IDX:
            node->item->status = parse_status(update_str); // No free needed because its not a heap string
        break;
    }

    /* =============== Step 5 =============== */
    char buffer[1024];
    sprintf(buffer, "%d;%s;%s;%s;%s",
                    uid, 
                    node->item->owner, 
                    node->item->description, 
                    node->item->location,
                    status_to_string(node->item->status));

    registry_command_reinsert(reg, buffer);

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
    // Print left subtree first using recursion
    print_in_order(root->left_arr[attribute_index], attribute_index);

    // Print current node
    print_item_line(root->item);

    // Print right subtree using recursion AFTER!
    print_in_order(root->right_arr[attribute_index], attribute_index);
}

int count_total_nodes(bst_node_t *root, int attribute_index){
    // Using recursion to make sure ALL subtrees are counted
    if (root == NULL){
        return 0;
    }

    // Ensures we count each left and right subtree by going left and right at each node
    return 1 + count_total_nodes(root->left_arr[attribute_index], attribute_index)
             + count_total_nodes(root->right_arr[attribute_index], attribute_index);
}

void count_unique_vals(bst_node_t *root, int attribute_index, item_t **prev_item, int *cnt){
    // Stop segfault from happening in line 797 (make recursion do-able)
    if (root == NULL){
        return;
    }

    // Special case/s (UID, empty tree)
    if (attribute_index == 0){
        *cnt = count_total_nodes(root, attribute_index);
        return;
    } 

    // Traverse left sub-tree first
    /* Here we traverse the left sub trees first (untill all left branches have been traversed through) because our
    BST is sorted with lower vals on the left. Therefore, because we want to check whether adjacent sorted vals are 
    unique (best way to implement uniqueness check) we first start at lowest ("left-lowest") branch */
    count_unique_vals(root->left_arr[attribute_index], attribute_index, prev_item, cnt);

    /* Check current s.t. here:
    diff = 0 => not different
    diff = 1 => different */
    int diff = 0;

    // Check to see if prev_item is NULL (first item is always unique)
    if (*prev_item == NULL){
        diff = 1;
    } else {
        
        switch(attribute_index){
            // Owner
            case ITEM_OWNER_IDX:
                if (strcmp(root->item->owner, (*prev_item)->owner) != 0){
                    diff = 1;
                }
            break;

            // Description
            case ITEM_DESCRIPTION_IDX:
                if (strcmp(root->item->description, (*prev_item)->description) != 0){
                    diff = 1;
                }
            break;

            // Location
            case ITEM_LOCATION_IDX:
                if (strcmp(root->item->location, (*prev_item)->location) != 0){
                    diff = 1;
                }
            break;
            
            // Status
            // Note: status is an enum (which is integers) so no strcmp here
            case ITEM_STATUS_IDX:
                if (root->item->status != (*prev_item)->status){
                    diff = 1;
                }
            break;
        }
    }
    
    // Update count
    if (diff){
        (*cnt)++;
    }

    // Update previous item
    *prev_item = root->item;

    // Now we traverse right subtree
    count_unique_vals(root->right_arr[attribute_index], attribute_index, prev_item, cnt);
}

int count_tree_height(bst_node_t *root, int attribute_index){
    // Use recursion to count the number of traversals 
    if (root == NULL){
        return 0;
    }

    int left_height = count_tree_height(root->left_arr[attribute_index], attribute_index);
    int right_height = count_tree_height(root->right_arr[attribute_index], attribute_index);

    // Height will be the largest sub-tree "route"
    return 1 + (left_height > right_height ? left_height : right_height);
}

void query_bst(bst_node_t *root, int attribute_index, char *query_str){
    /* =============== Goal =============== 
    Print succesful matches with queried item property
    This is done through in order traversal as required */

    // End of the BST tree
    if (root == NULL){
        return;
    }

    // Traverse left tree first (printing in order)
    query_bst(root->left_arr[attribute_index], attribute_index, query_str);

    // Check current node
    switch(attribute_index){
        // UID check
        case ITEM_ID_IDX:
            if (root->item->uid == atoi(query_str)){
                printf(QUERY_FOUND_STR, query_str);
                print_item_line(root->item);
            }
        break;
        
        // Owner check
        case ITEM_OWNER_IDX:
            if (strcmp(root->item->owner, query_str) == 0){
                printf(QUERY_FOUND_STR, query_str);
                print_item_line(root->item);
            }
        break;

        // Description check
        case ITEM_DESCRIPTION_IDX:
            if (strcmp(root->item->description, query_str) == 0){
                printf(QUERY_FOUND_STR, query_str);
                print_item_line(root->item);
            }
        break;

        // Location check
        case ITEM_LOCATION_IDX:
            if (strcmp(root->item->location, query_str) == 0){
                printf(QUERY_FOUND_STR, query_str);
                print_item_line(root->item);
            }
        break;

        // Status check
        case ITEM_STATUS_IDX:
            if (root->item->status == parse_status(query_str)){
                printf(QUERY_FOUND_STR,query_str);
                print_item_line(root->item);
            }
        break;
    }

    // Now traverse right tree
    query_bst(root->right_arr[attribute_index], attribute_index, query_str);
}

void find_uid_helper(bst_node_t **curr_ptr, item_t *item, bst_node_t **node, bst_node_t ***parent_ptr, int attribute_index){
    /* =============== Goal =============== 
    Find item with matching UID */

    // End of branch
    if (*curr_ptr == NULL){
        return;
    }

    // Found matching UID
    if ((*curr_ptr)->item->uid == item->uid){
        *node = *curr_ptr;
        *parent_ptr = curr_ptr;
        return; // Note: we can return here because we know UID's are unique 
    }

    // Search left subtree 
    find_uid_helper(&((*curr_ptr)->left_arr[attribute_index]), item, node, parent_ptr, attribute_index);

    // If UID is already found 
    // (NOTE: we initially pass *node == NULL so if *node != NULL => uid already found as per block above)
    /* Also note we do this after the check of the curr node and after moving left because our traversal is s.t.
    Search left subtree
    if found => STOP
    if not found search right subtree */
    if (*node != NULL){
        return;
    }

    // Search right subtree
    find_uid_helper(&((*curr_ptr)->right_arr[attribute_index]), item, node, parent_ptr, attribute_index);
}

void registry_command_reinsert(registry_t *reg, char *command_str){
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
        printf(UPDATE_SUCCESS_STR, uid);
        print_item_line(item);
    } else {
        printf(UPDATE_FAILURE_STR, uid);
        return;
    }
}