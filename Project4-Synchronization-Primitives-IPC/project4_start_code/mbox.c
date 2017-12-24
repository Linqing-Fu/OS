#include "common.h"
#include "mbox.h"
#include "sync.h"
#include "scheduler.h"


typedef struct
{
	/* TODO */
  char message[MAX_MESSAGE_LENGTH]; 
}Message;

typedef struct
{
	/* TODO */
  char name[MBOX_NAME_LENGTH];
  Message box[MAX_MBOX_LENGTH];
  int use_num;
  int ;
  lock_t l;
  node_t send_queue;
  node_t recv_queue;
  int write;
  int read;
  int msg_count;
} MessageBox;


static MessageBox MessageBoxen[MAX_MBOXEN];
lock_t BoxLock;

/* Perform any system-startup
 * initialization for the message
 * boxes.
 */
void init_mbox(void)
{
	/* TODO */
  int i;
  for (i = 0; i < MAX_MBOXEN; i++){
    MessageBoxen[i].use_num = 0;
    MessageBoxen[i].name[0] = '\0';
    MessageBoxen[i].write = 0;
    MessageBoxen[i].read = 0;
    MessageBoxen[i].msg_count = 0;
    lock_init(&MessageBoxen[i].l);
    queue_init(&MessageBoxen[i].send_queue);
    queue_init(&MessageBoxen[i].recv_queue);
  }
}

/* Opens the mailbox named 'name', or
 * creates a new message box if it
 * doesn't already exist.
 * A message box is a bounded buffer
 * which holds up to MAX_MBOX_LENGTH items.
 * If it fails because the message
 * box table is full, it will return -1.
 * Otherwise, it returns a message box
 * id.
 */
mbox_t do_mbox_open(const char *name)
{
  (void)name;
	/* TODO */
  int i;
  for (i = 0; i < MAX_MBOXEN; i++){
    if(same_string(MessageBoxen[i].name, name)){
      current_running->mailbox[i] = TRUE;
      return i;

    } else if (strlen(MessageBoxen[i].name) == 0){
      bcopy(name, MessageBoxen[i].name, strlen(name));
      current_running->mailbox[i] = TRUE;
      return i;
    }
  }
  return -1;
}

/* Closes a message box
 */
void do_mbox_close(mbox_t mbox)
{
  (void)mbox;
	/* TODO */
  MessageBoxen[mbox].use_num --;
  if (MessageBoxen[mbox].use_num == 0){
    MessageBoxen[mbox].use_num = 0;
    MessageBoxen[mbox].name[0] = '\0';
    MessageBoxen[mbox].write = 0;
    MessageBoxen[mbox].read = 0;
    lock_init(&MessageBoxen[mbox].l);
    queue_init(&MessageBoxen[mbox].send_queue);
    queue_init(&MessageBoxen[mbox].recv_queue);
  }

}

/* Determine if the given
 * message box is full.
 * Equivalently, determine
 * if sending to this mbox
 * would cause a process
 * to block.
 */
int do_mbox_is_full(mbox_t mbox)
{
  (void)mbox;
	/* TODO */
  if (MessageBoxen[mbox].msg_count == MAX_MBOX_LENGTH){
    enter_critical();
    block(&MessageBoxen[mbox].send_queue);
    leave_critical();
    return 1;
  } else {
    return 0;
  }
}

int do_mbox_is_empty(mbox_t mbox)
{
  (void)mbox;
  /* TODO */
  if (MessageBoxen[mbox].msg_count == 0){
    enter_critical();
    block(&MessageBoxen[mbox].recv_queue);
    leave_critical();
    return 1;
  } else {
    return 0;
  }
}

/* Enqueues a message onto
 * a message box.  If the
 * message box is full, the
 * process will block until
 * it can add the item.
 * You may assume that the
 * message box ID has been
 * properly opened before this
 * call.
 * The message is 'nbytes' bytes
 * starting at offset 'msg'
 */
void do_mbox_send(mbox_t mbox, void *msg, int nbytes)
{
  (void)mbox;
  (void)msg;
  (void)nbytes;
  int i;
  lock_t *temp;
  /* TODO */
  do_mbox_is_full(mbox);

  lock_acquire(&MessageBoxen[mbox].l);
  for(i = 0; i < nbytes && i < MAX_MESSAGE_LENGTH; i++)
    MessageBoxen[mbox].box[MessageBoxen[mbox].write].message[i] = *((char *)msg + i);
  
  MessageBoxen[mbox].write ++;
  MessageBoxen[mbox].write = MessageBoxen[mbox].write % MAX_MBOX_LENGTH;
  MessageBoxen[mbox].msg_count ++;

  lock_release(&MessageBoxen[mbox].l);
  enter_critical();
  unblock_all(&MessageBoxen[mbox].recv_queue);
  leave_critical();
}

/* Receives a message from the
 * specified message box.  If
 * empty, the process will block
 * until it can remove an item.
 * You may assume that the
 * message box has been properly
 * opened before this call.
 * The message is copied into
 * 'msg'.  No more than
 * 'nbytes' bytes will by copied
 * into this buffer; longer
 * messages will be truncated.
 */
void do_mbox_recv(mbox_t mbox, void *msg, int nbytes)
{
  (void)mbox;
  (void)msg;
  (void)nbytes;
  /* TODO */

  char *s;
  do_mbox_is_empty(mbox);

  lock_acquire(&MessageBoxen[mbox].l);
  int i;
  for(i = 0; i < nbytes && i < MAX_MESSAGE_LENGTH; i++)
    *((char *)msg + i) = MessageBoxen[mbox].box[MessageBoxen[mbox].read].message[i];
  

  //bcopy(MessageBoxen[mbox].box[MessageBoxen[mbox].read], s, nbytes);

  MessageBoxen[mbox].read ++;
  MessageBoxen[mbox].read = MessageBoxen[mbox].read % MAX_MBOX_LENGTH;
  MessageBoxen[mbox].msg_count --;

  lock_release(&MessageBoxen[mbox].l);
  enter_critical();
  unblock_all(&MessageBoxen[mbox].send_queue);
  leave_critical();
}


