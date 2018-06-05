/**
 * @file vmaccess.c
 * @author Prof. Dr. Wolfgang Fohl, HAW Hamburg
 * @date 2010
 * @brief The access functions to virtual memory.
 */

#include "vmaccess.h"
#include <sys/ipc.h>
#include <sys/shm.h>

#include "syncdataexchange.h"
#include "vmem.h"
#include "debug.h"

/*
 * static variables
 */

static struct vmem_struct *vmem = NULL; //!< Reference to virtual memory

/**
 * The progression of time is simulated by the counter g_count, which is incremented by 
 * vmaccess on each memory access. The memory manager will be informed by a command, whenever 
 * a fixed period of time has passed. Hence the memory manager must be informed, whenever 
 * g_count % TIME_WINDOW == 0. 
 * Based on this information, memory manager will update aging information
 */

static int g_count = 0;    //!< global acces counter as quasi-timestamp - will be increment by each memory access
int ref_c = 0;
#define TIME_WINDOW   20

/**
 *****************************************************************************************
 *  @brief      This function setup the connection to virtual memory.
 *              The virtual memory has to be created by mmanage.c module.
 *
 *  @return     void
 ****************************************************************************************/
static void vmem_init(void) {

    /* Create System V shared memory */

	key_t key;

	key = ftok(SHMKEY, SHMPROCID);
	TEST_AND_EXIT(key > 0, (stderr, "Didnt get a valid key!"));

	/* We are only using the shm, don't set the IPC_CREAT flag */
	int shm_id = shmget(key, SHMSIZE, 0666);
	TEST_AND_EXIT(shm_id < 0, (stderr, "Didnt get a valid shared memory ID!"));

	/* attach shared memory to vmem */
	vmem = (struct vmem_struct *) shmat(shm_id, NULL, 0);

	/* Parameter false, da es sich bei dem Modul vmappl + vmaccess um den "Client" handelt. */
	//setupSyncDataExchange();

}

/**
 *****************************************************************************************
 *  @brief      This function puts a page into memory (if required). Ref Bit of page table
 *              entry will be updated.
 *              If the time window handle by g_count has reached, the window window message
 *              will be send to the memory manager. 
 *              To keep conform with this log files, g_count must be increased before 
 *              the time window will be checked.
 *              vmem_read and vmem_write call this function.
 *
 *  @param      address The page that stores the contents of this address will be 
 *              put in (if required).
 * 
 *  @return     void
 ****************************************************************************************/
static void vmem_put_page_into_mem(int pageno) {
	struct msg message = {CMD_PAGEFAULT, pageno, g_count, 0};

	sendMsgToMmanager(message);

}

int vmem_read(int address) {
	TEST_AND_EXIT(address < 0, (stderr, "Got negative address!"));

	if(vmem == NULL) { vmem_init(); }

	int pageno = address / VMEM_PAGESIZE;
	int offset = address % VMEM_PAGESIZE;

	TEST_AND_EXIT(pageno >= VMEM_NPAGES, (stderr, "Calculated false page number!"));

	if((vmem->pt[pageno].flags & PTF_PRESENT) == 0)
	{
		vmem_put_page_into_mem(pageno);
	}

	vmem->pt[pageno].flags |= PTF_REF;

	int result = vmem->mainMemory[vmem->pt[pageno].frame * VMEM_PAGESIZE + offset];

	g_count++;
	if(g_count % TIME_WINDOW == 0)
	{
		struct msg message;
		message.cmd = CMD_TIME_INTER_VAL;
		message.g_count = g_count;

		sendMsgToMmanager(message);
	}
	return result;
}

void vmem_write(int address, int data) {
	TEST_AND_EXIT(address < 0, (stderr, "Got negative address!"));

	if(vmem == NULL) { vmem_init(); }

	int pageno = address / VMEM_PAGESIZE;
	int offset = address % VMEM_PAGESIZE;

	TEST_AND_EXIT(pageno >= VMEM_NPAGES, (stderr, "Calculated false page number!"));

	// check if the needed page is available in page table
	if((vmem->pt[pageno].flags & PTF_PRESENT) == 0)
	{
		vmem_put_page_into_mem(pageno);
	}

	//set R flag
	vmem->pt[pageno].flags |= PTF_REF;
	vmem->pt[pageno].flags |= PTF_DIRTY;

	//do actually write
	vmem->mainMemory[vmem->pt[pageno].frame * VMEM_PAGESIZE + offset] = data;
	//vmem->mainMemory[address] = data;
	//increment system clocks
	g_count++;
	if(g_count % TIME_WINDOW == 0)
	{
		struct msg message;
		message.cmd = CMD_TIME_INTER_VAL;
		message.g_count = g_count;

		sendMsgToMmanager(message);
	}
}
// EOF
