<html>

<head>
</head>

<body lang=RU style='tab-interval:35.4pt'>

<div class=WordSection1>

<p class=MsoNormal><span lang=EN-US style='mso-ansi-language:EN-US'>2000-2020 (c) Dmitry <span
class=SpellE>Blyakhman</span>, <span class=SpellE>Yakov</span> <span
class=SpellE>Starikov</span> <o:p></o:p></span></p>

<p class=MsoNormal><span lang=EN-US style='mso-ansi-language:EN-US'>Technology of functional blocks (<span class=SpellE>fb</span>-core-lib system library) - programming
technology for multitasking applications.<o:p></o:p></span></p>

<p class=MsoNormal><span lang=EN-US style='mso-ansi-language:EN-US'>Developers
of multitasking applications (including real-time ones) often face the problem
of exchanging events and data between different threads and clearly
synchronizing the work of these threads with each other. For such applications
to work correctly, it requires the creation of a large number of semaphores and
mailboxes, or the use of operating system mechanisms for exchanging events.
This rather painstaking task, the development and debugging of such
applications, takes a lot of programmers ' time.<o:p></o:p></span></p>

<p class=MsoNormal><span lang=EN-US style='mso-ansi-language:EN-US'>The
presented technology was developed to create software for automatic telephone
exchanges (PBX) in 2000 and for a period of more than 20 years has shown quite
high efficiency. It is applicable in real-time operating systems, as well as in
applications written for Linux and Windows, the development of high-load systems
and <span class=SpellE>microservices</span>. Applications with thousands of
functioning interacting blocks are being developed, the speed of development,
debugging, stability of such programs has increased, and reliability has
significantly increased.<o:p></o:p></span></p>

<p class=MsoNormal><span lang=EN-US style='mso-ansi-language:EN-US'>The entire
software system is an extended finite state machine. It consists of a set of
modules, functional blocks that work as independent tasks (threads) and
interact with each other using a message sending mechanism. Each block can
include several other functional blocks, thus ensuring the modularity of the
program. The development of such modules can be carried out independently by
coordinating the interfaces of each block (a set of received and sent events
and data). The behavior of each block is determined by an extended finite state
machine that performs actions and generates reactions (events) in response to
external discrete influences (events).<o:p></o:p></span></p>

<p class=MsoNormal><span lang=EN-US style='mso-ansi-language:EN-US'>The entire
software system is built from a set of functional blocks inherited from the <span
class=SpellE>FuncBlock</span> class.<o:p></o:p></span></p>

<p class=MsoNormal><span lang=EN-US style='mso-ansi-language:EN-US'>Each
functional block (the building module of the entire software system) has a
virtual task () method - the main method of the functional block that
implements the logic of its functioning (a finite state machine). The <span
class=GramE>task(</span>) method works as a separate thread (in terms of
multitasking operating systems).<o:p></o:p></span></p>

<p class=MsoNormal><span lang=EN-US style='mso-ansi-language:EN-US'>The
function block has an event buffer (mailbox) for exchanging events and data
with other blocks. To exchange events with another function block, this block
puts an event in the event buffer of the block to which it wants to send the
event. When any event is received, the task () function is activated, which
processes this event in a certain way. A function block has a limited set of
events that it can process (input) and a set of events that it can send to
other blocks (output). Events are processed by the function block one at a time
in the order in which they are received. A functional block has a finite number
of states, in each of which it can receive a number of valid events sent to
this block. The block can be in one of the states or in a transition between
states. If an event intended for this block arrives during the transition, it
is placed in the event buffer. If there are no events in the receiving buffer,
the <span class=GramE>task(</span>) function is temporarily blocked (it does
not waste CPU resources) until the next event arrives or the timeout expires.
The actions performed during the transition can consist in converting data,
sending events to other blocks, etc.<o:p></o:p></span></p>

<p class=MsoNormal><span lang=EN-US style='mso-ansi-language:EN-US'>The <span
class=SpellE>MultiThreadFBExecutor</span> class implements the execution of <span
class=GramE>task(</span>) functions for all function blocks with a dedicated
number of threads (the number of allocated threads for the executor is set in
its constructor). If there are not enough free threads, the functional block is
queued for execution. <span class=SpellE>MultiThreadFBExecutor</span> must be
created in a single instance to execute all function blocks. If there are no
events in the system, the executor stops at the semaphore and does not waste
processor resources.<o:p></o:p></span></p>

<p class=MsoNormal><span lang=EN-US style='mso-ansi-language:EN-US'>In
addition, a functional block can have a set of timers that allow the block to
work in real time, as well as to withstand certain timeouts. The number of
timers in the function block is not limited. Timers can have the ONE_TIME and
PERIODIC types. To create a timer, the executor method <span class=SpellE>FBTimer</span>
= <span class=SpellE><span class=GramE>startTimer</span></span><span
class=GramE>(</span><span class=SpellE>fbName,timerType</span>, <span
class=SpellE>callbackEvent</span>) is used, for early deletion of the timer,
the <span class=SpellE>deleteTimer</span>(<span class=SpellE>FBTimer</span>)
method is used. When the timer is triggered, the executor sends the <span
class=SpellE>callbackEvent</span> event to the function block with the name <span
class=SpellE>fbName</span>.<o:p></o:p></span></p>

<p class=MsoNormal><span lang=EN-US style='mso-ansi-language:EN-US'>To
temporarily stop the operation of all functional blocks (for example, to
dynamically reset the program settings), the executor methods <span
class=SpellE><span class=GramE>stopSystem</span></span><span class=GramE>(</span>)
and <span class=SpellE>releaseSystem</span> () are implemented. When executing
the <span class=SpellE>stopSystem</span> () method, the executor stops the
execution of all function blocks and waits for the completion of all task ()
functions. After that, you can dynamically re-read all the program settings and
call the <span class=SpellE><span class=GramE>releaseSystem</span></span><span
class=GramE>(</span>) method to start the executor again.<o:p></o:p></span></p>

<p class=MsoNormal><span lang=EN-US style='mso-ansi-language:EN-US'>You can
create static functional blocks, as well as dynamic blocks (they are created
and destroyed during the program operation). To dynamically delete a block from
the system, it must call its <span class=SpellE>markForDelete</span> () method,
when the <span class=GramE>task(</span>) function completes, the block will be
deleted.<o:p></o:p></span></p>

<p class=MsoNormal><span lang=EN-US style='mso-ansi-language:EN-US'>You can
combine the work of functional blocks and ordinary program threads.<o:p></o:p></span></p>

<p class=MsoNormal><span lang=EN-US style='mso-ansi-language:EN-US'>It is also
possible to implement a mechanism for automatically tracing the operation of
the software system (there is no library in this version).<o:p></o:p></span></p>

<p class=MsoNormal><span lang=EN-US style='mso-ansi-language:EN-US'>An example
of a block diagram of software written using this technology shows the
structure of the program, a set of events and the interaction of modules
(functional blocks) with each other.<o:p></o:p></span></p>

<p class=MsoNormal><span lang=EN-US style='mso-ansi-language:EN-US'><o:p>&nbsp;</o:p></span></p>
<img src="http://3.bp.blogspot.com/-2-rB1n9r0Z8/T30my6lY-RI/AAAAAAAAAGs/bBDg1QIeEEE/s1600/fb2.jpg" alt="An example
of a block diagram of software written using this technology">
<p class=MsoNormal><span lang=EN-US style='mso-ansi-language:EN-US'><o:p>&nbsp;</o:p></span></p>

<p class=MsoNormal><span lang=EN-US style='mso-ansi-language:EN-US'>This
technology allows you to use modern software development tools, such as
graphical CASE tools, for automatic generation of the source code of large
finite automata-task () methods.<o:p></o:p></span></p>

<p class=MsoNormal><span lang=EN-US style='mso-ansi-language:EN-US'>You can
create multitasking platform-independent applications by rewriting the system
library for a specific operating system and programming language, the upper
application levels will remain unchanged and will be easily ported.<o:p></o:p></span></p>

<p class=MsoNormal><span lang=EN-US style='mso-ansi-language:EN-US'>Currently, the
library versions for Java and C+<span class=GramE>+(</span>Linux) are
implemented.<o:p></o:p></span></p>
  
<a href="https://github.com/dblyakhman/fb-core-lib-java">fb-core-lib-java</a>

</div>

</body>

</html>
