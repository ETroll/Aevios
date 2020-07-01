#include "util.h"

util_list* efi_util_list_new(EFI_SYSTEM_TABLE* ST)
{
    util_list* list;
    EFI_STATUS status = ST->BootServices->AllocatePool(EfiLoaderData, sizeof(util_list), (void**)&list);

    if(EFI_ERROR(status))
    {
        return NULL;
    }

    list->count = 0;
    list->root = (util_listnode*)NULL;

    return list;
}

void efi_util_list_delete(EFI_SYSTEM_TABLE* ST, util_list* list)
{
    if(list != NULL) 
    {
        if(list->root != NULL) 
        {
            util_listnode* itterator = list->root;
            while(itterator->next)
            {
                util_listnode* node = itterator;
                itterator = itterator->next;

                efi_util_listnode_delete(ST, node);
            }
        }
        ST->BootServices->FreePool(list);
    }
}

int efi_util_list_add(EFI_SYSTEM_TABLE* ST, util_list* list, void* data)
{
    util_listnode* node = efi_util_listnode_new(ST, data);
    if(node == NULL)
    {
        return 0;
    }

    if(list->root == NULL)
    {
        list->root = node;
    }
    else
    {
        util_listnode* itterator = list->root;
        while(itterator->next)
        {
            itterator = itterator->next;
        }
        itterator->next = node;
        node->prev = itterator;
    }
    list->count++;

    return 1;
}

void* efi_util_list_getAt(util_list* list, unsigned int index)
{
    if(list->count == 0 || index >= list->count)
    {
        return (void*)NULL;
    }
    util_listnode* itterator = list->root;

    for(unsigned int i = 0; (i < index) && itterator; i++)
    {
        itterator = itterator->next;
    }
    return itterator != NULL ? itterator->data : (void*)NULL;
}

util_listnode* efi_util_listnode_new(EFI_SYSTEM_TABLE* ST, void* data)
{
    util_listnode* node;
    EFI_STATUS status = ST->BootServices->AllocatePool(EfiLoaderData, sizeof(util_listnode), (void**)&node);
    
    if(EFI_ERROR(status))
    {
        return NULL;
    }

    node->prev = (util_listnode*)NULL;
    node->next = (util_listnode*)NULL;
    node->data = data; 

    return node;
}

void efi_util_listnode_delete(EFI_SYSTEM_TABLE* ST, util_listnode* node)
{
    if(node != NULL)
    {
        ST->BootServices->FreePool(node);
    }
}

