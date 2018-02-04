#include "const.h"
#include "type.h"
#include "proto.h"
#include "global.h"


/*******************
 * in this file we define these functions
 * ####################################################
 * PUBLIC	void	cstart()
 * 
 * 
 * 
 * 
 * 
 * 
 * ####################################################
 *  * and we define these global variables
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * *****************/



PUBLIC void cstart()
{
	DispStr("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
		 "-----\"cstart\" begins-----\n");




//GDT
	/* 将 LOADER 中的 GDT 复制到新的 GDT 中 */
	MemCopy(&gdt,				  /* New GDT */
	       (void*)(*((u32*)(&gdt_ptr[2]))),   /* Base  of Old GDT */
	       *((u16*)(&gdt_ptr[0])) + 1	  /* Limit of Old GDT */
		);
	/* gdt_ptr[6] 共 6 个字节：0~15:Limit  16~47:Base。用作 sgdt/lgdt 的参数。*/
	//init GDT
	//DispStr("-----init GDT----- \n");

	u16* p_gdt_limit = (u16*)(&gdt_ptr[0]);
	u32* p_gdt_base  = (u32*)(&gdt_ptr[2]);
	*p_gdt_limit = GDT_SIZE * sizeof(Descriptor) - 1;
	*p_gdt_base  = (u32)&gdt;

	//DispStr("-----init GDT OK----- \n");
//IDT
	//init IDT
	//DispStr("-----init IDT----- \n");
	u16* p_idt_limit = (u16*)(&idt_ptr[0]);
	u32* p_idt_base = (u32*)(&idt_ptr[2]);
	*p_idt_limit = IDT_SIZE*sizeof(Gate)-1;
	*p_idt_base = (u32)&idt;

	//DispStr("-----init IDT OK----- \n");





	//DispStr("-----init protect mode----- \n");
	initProtect();
	//DispStr("-----init protect mode OK----- \n");



	DispStr("-----\"cstart\" ends-----\n");
}