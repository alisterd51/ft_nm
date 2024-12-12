#include <elf.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

typedef struct s_list t_list;
struct s_list
{
    void *content;
    t_list *next;
};

typedef struct s_symbol t_symbol;
struct s_symbol
{
    unsigned long value;
    char type;
    char *name;
};

void ft_lstadd_back(t_list **lst, t_list *new)
{
    while (*lst)
    {
        lst = &((*lst)->next);
    }
    *lst = new;
}

void ft_lstadd_front(t_list **lst, t_list *new)
{
    new->next = *lst;
    *lst = new;
}

t_list *ft_lstnew(void *content)
{
    t_list *new_lst = malloc(sizeof(t_list));

    if (!new_lst)
    {
        return (NULL);
    }
    *new_lst = (t_list){.content = content, .next = NULL};
    return (new_lst);
}

void ft_lstdelone(t_list *lst, void (*del)(void *))
{
    if (del && lst)
    {
        del(lst->content);
    }
    free(lst);
}

void ft_lstclear(t_list **lst, void (*del)(void *))
{
    t_list *current = *lst;

    while (current)
    {
        t_list *element = current;

        current = current->next;
        ft_lstdelone(element, del);
    }
    *lst = NULL;
}

char get_symbol_type_64(Elf64_Sym *sym)
{
    // a partir de sym->st_info on doit recup le char:
    // A B b C c D d G g i I N n p R r S s T t U u V v W w - ?
    unsigned char st_bind = ELF64_ST_BIND(sym->st_info);
    // unsigned char st_type = ELF64_ST_TYPE(sym->st_info);
    if (st_bind == STB_WEAK)
    {
        return ('w');
    }
    return ('.');
}

void free_symbol(void *ptr)
{
    t_symbol *symbol = ptr;

    free(symbol->name);
    free(symbol);
}

t_list *get_list_symbol(char *ptr)
{
    Elf64_Ehdr *elf_file_header = (Elf64_Ehdr *)ptr;
    Elf64_Shdr *section_header = (Elf64_Shdr *)(ptr + elf_file_header->e_shoff);
    Elf64_Shdr *symtab = (Elf64_Shdr *)NULL;
    Elf64_Shdr *strtab = (Elf64_Shdr *)NULL;
    char *str = (ptr + section_header[elf_file_header->e_shstrndx].sh_offset);

    for (unsigned int i = 0; i < elf_file_header->e_shnum; i++)
    {
        if (strcmp(&str[section_header[i].sh_name], ".symtab") == 0)
        {
            symtab = &section_header[i];
        }
        else if (strcmp(&str[section_header[i].sh_name], ".strtab") == 0)
        {
            strtab = &section_header[i];
        }
    }

    if (symtab != NULL && strtab != NULL)
    {
        Elf64_Sym *sym = (Elf64_Sym *)(ptr + symtab->sh_offset);
        char *str = (ptr + strtab->sh_offset);
        t_list *lst_symbol = NULL;

        for (unsigned int i = 0; i < symtab->sh_size / sizeof(Elf64_Sym); i++)
        {
            t_symbol *symbol = malloc(sizeof(t_symbol));
            
            if (symbol == NULL) {
                ft_lstclear(&lst_symbol, free_symbol);
                return (NULL);
            }
            *symbol = (t_symbol){
                .value = sym[i].st_value,
                .type = get_symbol_type_64(&sym[i]),
                .name = strdup(str + sym[i].st_name),
            };
            t_list *new_symbol = ft_lstnew(symbol);
            ft_lstadd_back(&lst_symbol, new_symbol);
        }
        return (lst_symbol);
    }
    return (NULL);
}

void print_list_symbol(t_list *lst_symbol) {
    for (t_list *symbol = lst_symbol; symbol != NULL; symbol = symbol->next)
    {
        t_symbol *sym = symbol->content;
        if (sym->value)
        {
            printf("%017lx ", sym->value);
        }
        else
        {
            printf("                  ");
        }
        printf("%c %s\n", sym->type, sym->name);
    }
}

void handle_64(char *ptr)
{
    t_list *lst_symbol = get_list_symbol(ptr);

    print_list_symbol(lst_symbol);
    ft_lstclear(&lst_symbol, free_symbol);
}

void ft_nm(char *ptr)
{
    // compare the first 4 bytes of ptr and magic number elf 64
    if (*(int *)ptr == *(int *)ELFMAG && ptr[4] == ELFCLASS32)
    {
        printf("elf32 not suported\n");
    }
    else if (*(int *)ptr == *(int *)ELFMAG && ptr[4] == ELFCLASS64)
    {
        handle_64(ptr);
    }
    else
    {
        printf("file format not recognized\n");
    }
}

int map_file(char *filename)
{
    int fd = open(filename, O_RDONLY);
    if (fd < 0)
    {
        perror("open");
        return (EXIT_FAILURE);
    }

    struct stat buf;
    if (fstat(fd, &buf) < 0)
    {
        perror("fstat");
        return (EXIT_FAILURE);
    }

    char *ptr = mmap(0, buf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    close(fd);
    if (ptr == MAP_FAILED)
    {
        perror("mmap");
        return (EXIT_FAILURE);
    }

    ft_nm(ptr);
    if (munmap(ptr, buf.st_size) < 0)
    {
        perror("munmap");
        return (EXIT_FAILURE);
    }

    return (EXIT_SUCCESS);
}

int main(int ac, char **av)
{
    int ret_value = EXIT_SUCCESS;

    if (ac == 1)
    {
        ret_value = map_file("a.out");
    }
    else
    {
        for (int c = 1; c < ac; c++)
        {
            ret_value |= map_file(av[c]);
        }
    }
    return (ret_value);
}
