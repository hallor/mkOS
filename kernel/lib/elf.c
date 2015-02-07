/*
  Copyright (C) 2015 Mateusz Kulikowski <mateusz.kulikowski@gmail.com>

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdint.h>
#include "printk.h"
#include "elf64.h"
#include "util.h"
#include "task.h"
#include "page.h"

static const char *pheader_types[] =
{
    "Unused",
    "Load",
    "Dynamic",
    "Interpreter",
    "Note",
    "Reserved (shlib)",
    "Program Header",
    "Thread-Local-Storage"
};

static const char * str_perms(uint16_t perms)
{
    static char p[4]={0,};
    if (perms & PF_X)
        p[2] = 'x';
    else
        p[2] = '-';
    if (perms & PF_W)
        p[1] = 'w';
    else
        p[1] = '-';
    if (perms & PF_R)
        p[0] = 'r';
    else
        p[0] = '-';
    return p;
}

static void parse_program_header(struct task * t,void * blob, unsigned size, unsigned num, unsigned hdr_offset)
{
    dbg("Parsing program header. size: %d number: %d\n", size, num);
    Elf64_Phdr * hdr = blob;
    if (size != sizeof(Elf64_Phdr))
        panic("Wrong program header size.\n");
    while (num--) {
    dbg("%s perms: %s file_ofs: %#llx",
         pheader_types[hdr->p_type], str_perms(hdr->p_flags), hdr->p_offset);
    printk(" paddr: %#llx vaddr: %#llx file_size: %#llx mem_size: %#llx align: %#llx\n",
         hdr->p_paddr, hdr->p_vaddr, hdr->p_filesz, hdr->p_memsz, hdr->p_align);
    if (hdr->p_type == PT_LOAD) {
        if (t->vma_addr == 0) {
            t->vma_size = ALIGN_PGUP(hdr->p_memsz);
            t->vma_addr = page_alloc(t->vma_size >> PAGE_SHIFT);
            info("Loading program... %d bytes to %d (%d) byte mem blob at %#x\n",
                 hdr->p_filesz, hdr->p_memsz, t->vma_size, t->vma_addr);
            memcpy(t->vma_addr, blob + hdr->p_offset - hdr_offset, hdr->p_filesz);
            memset(t->vma_addr + hdr->p_filesz, 0, t->vma_size - hdr->p_filesz);
        }
    }
    hdr ++;
    }
}

static const char * sheader_types[] =
{
    "NULL",
    "progbits",
    "symtab",
    "strtab",
    "rela",
    "hash",
    "dynamic",
    "note",
    "nobits",
    "rel",
    "res(shlib)",
    "dynsym",
    "init_array",
    "fini_array",
    "preinit_array",
    "group",
    "symtab_shndx",
};

static const char *get_sheader_type(unsigned type)
{
    if (type < (sizeof(sheader_types)/sizeof(sheader_types[0])))
        return sheader_types[type];
    return "???";
}

static void parse_section_header(void * blob, unsigned size, unsigned num, unsigned stridx)
{
    info("Parsing section header. size: %d number: %d strings at section %d\n", size, num, stridx);
    if (sizeof(Elf64_Shdr) != size)
        panic("Wrong session header size.\n");
    Elf64_Shdr * hdr = blob;
    while (num --) {
        dbg("name: %d type: %s\n", hdr->sh_name, get_sheader_type(hdr->sh_type));
        hdr++;
    }
}

static void parse_elf(struct task * t, void * blob, unsigned size)
{
    Elf64_Ehdr *hdr = blob;
    dbg("Parsing ELF header @ %#llx; %d bytes\n", blob, size);
    if (size < sizeof(Elf64_Ehdr)) {
        wrn("Image too small.\n");
        goto fail;
    }

    if (!IS_ELF(*hdr)) {
        wrn("Image is not ELF.\n");
        goto fail;
    }

    if (hdr->e_ident[EI_CLASS] != ELFCLASS64) {
        wrn("Unsupported elf class\n");
        goto fail;
    }
    if (hdr->e_ident[EI_DATA] != ELFDATA2LSB) {
        wrn("Unsupported elf endiannes\n");
        goto fail;
    }
    dbg("ABI: %02x ABI_v: %02x \n", hdr->e_ident[EI_OSABI], hdr->e_ident[EI_ABIVERSION]);

    dbg("type: %hx machine: %hd version: %x\n", hdr->e_type, hdr->e_machine, hdr->e_version);
    if (hdr->e_machine != EM_AARCH64) {
        wrn("Unsupported target machine.\n");
        goto fail;
    }

    dbg("entry: %#llx program header starts at %#llx section header starts at %#llx\n",
         hdr->e_entry, hdr->e_phoff, hdr->e_shoff);
    dbg("arch_flags: %#x elf_header_size: %#hx\n", hdr->e_flags, hdr->e_ehsize);
    parse_program_header(t, blob + hdr->e_phoff, hdr->e_phentsize, hdr->e_phnum, hdr->e_phoff);
//    parse_section_header(blob + hdr->e_shoff, hdr->e_shentsize, hdr->e_shnum, hdr->e_shstrndx);
    return;

 fail:
    panic("And it failed...\n");
}

int load_elf(struct task * task, void * blob, unsigned size)
{
    info("Loading task %s; blob size: %d\n", task->name, size);
    parse_elf(task, blob, size);
    return task->vma_addr == 0;
}
