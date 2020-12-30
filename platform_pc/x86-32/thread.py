import gdb.printing
import gdb

THREAD_COUNT=15


class PrintThreads (gdb.Command):

  def __init__ (self):
    super (PrintThreads, self).__init__ ("print_threads", gdb.COMMAND_USER)

  def invoke (self, arg, from_tty):
    all_tasks = gdb.parse_and_eval('global_threads_sp')

    longType = gdb.lookup_type("long")

    stacks = [all_tasks[i]["stack"] for i in range(THREAD_COUNT)]
    ir = [all_tasks[i]["entry"] for i in range(THREAD_COUNT)]

    runningIndex = int(gdb.parse_and_eval("current_tid").format_string())

    PC_Stopped = int(gdb.parse_and_eval("switchcontext_thread_pc").cast(longType))

    PC = gdb.parse_and_eval('$pc')
    SP = gdb.parse_and_eval('$sp')

    for i in range(0,THREAD_COUNT):
        if i != runningIndex:
            if stacks[i] == 0:
                continue
            if ir[i] != 0:
                gdb.parse_and_eval("$pc=%d" % (ir[i]))
            else:    
                gdb.parse_and_eval("$pc=%d" % (PC_Stopped))
            gdb.parse_and_eval("$sp=%d" % (stacks[i]))
        else:
#        gdb.execute("set $pc=%d" % (PC))
#        gdb.execute("set $sp=%d" % (SP))
            gdb.parse_and_eval("$pc=%d" % (PC))
            gdb.parse_and_eval("$sp=%d" % (SP))
        print("[%d%s] \n" % (i, "*" if i == runningIndex else ""))
        gdb.execute("bt");
        print("=========================================================\n")

    gdb.parse_and_eval("$pc=%d" % (PC))
    gdb.parse_and_eval("$sp=%d" % (SP))


PrintThreads ()
