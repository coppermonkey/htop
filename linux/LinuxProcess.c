/*
htop - LinuxProcess.c
(C) 2014 Hisham H. Muhammad
Released under the GNU GPL, see the COPYING file
in the source distribution for its full text.
*/

#include "Process.h"

/*{

typedef struct LinuxProcess_ {
   Process super;
   IOPriority ioPriority;
} LinuxProcess;

}*/

/*
[1] Note that before kernel 2.6.26 a process that has not asked for
an io priority formally uses "none" as scheduling class, but the
io scheduler will treat such processes as if it were in the best
effort class. The priority within the best effort class will  be
dynamically  derived  from  the  cpu  nice level of the process:
io_priority = (cpu_nice + 20) / 5. -- From ionice(1) man page
*/
#define LinuxProcess_effectiveIOPriority(p_) (IOPriority_class(p_->ioPriority) == IOPRIO_CLASS_NONE ? IOPriority_tuple(IOPRIO_CLASS_BE, (p_->nice + 20) / 5) : p_->ioPriority)

IOPriority LinuxProcess_updateIOPriority(Process* this) {
   IOPriority ioprio = syscall(SYS_ioprio_get, IOPRIO_WHO_PROCESS, this->pid);
   this->ioPriority = ioprio;
   return ioprio;
}

bool LinuxProcess_setIOPriority(Process* this, IOPriority ioprio) {
   syscall(SYS_ioprio_set, IOPRIO_WHO_PROCESS, this->pid, ioprio);
   return (Process_updateIOPriority(this) == ioprio);
}

void LinuxProcess_writeField(Process* this, RichString* str, ProcessField field) {
   switch (field) {
   case IO_PRIORITY: {
      int klass = IOPriority_class(this->ioPriority);
      if (klass == IOPRIO_CLASS_NONE) {
         // see note [1] above
         snprintf(buffer, n, "B%1d ", (int) (this->nice + 20) / 5);
      } else if (klass == IOPRIO_CLASS_BE) {
         snprintf(buffer, n, "B%1d ", IOPriority_data(this->ioPriority));
      } else if (klass == IOPRIO_CLASS_RT) {
         attr = CRT_colors[PROCESS_HIGH_PRIORITY];
         snprintf(buffer, n, "R%1d ", IOPriority_data(this->ioPriority));
      } else if (this->ioPriority == IOPriority_Idle) {
         attr = CRT_colors[PROCESS_LOW_PRIORITY]; 
         snprintf(buffer, n, "id ");
      } else {
         snprintf(buffer, n, "?? ");
      }
      break;
   }
   default:
      snprintf(buffer, n, "- ");
   }
   RichString_append(str, attr, buffer);
   }
}

long LinuxProcess_compare(const void* v1, const void* v2) {
   LinuxProcess *p1, *p2;
   ProcessList *pl = ((Process*)v1)->pl;
   if (pl->direction == 1) {
      p1 = (LinuxProcess*)v1;
      p2 = (LinuxProcess*)v2;
   } else {
      p2 = (LinuxProcess*)v1;
      p1 = (LinuxProcess*)v2;
   }
   switch (pl->sortKey) {
   case IO_PRIORITY:
      return Process_effectiveIOPriority(p1) - Process_effectiveIOPriority(p2);
   default:
      return (p1->pid - p2->pid);
   }
   test_diff:
   return (diff > 0) ? 1 : (diff < 0 ? -1 : 0);
}
