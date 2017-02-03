# tests

This README aims to explain the testing and debugging that has went to build this shell.

* *args.cpp*
 <pre style="background: rgb(238, 238, 238); border: 1px solid rgb(204, 204, 204); padding: 5px 10px;">./args 1 2 3 4</pre>

 Expected output -
  <pre style="background: rgb(238, 238, 238); border: 1px solid rgb(204, 204, 204); padding: 5px 10px;">You entered 6 arguments>./args<	>1<	>2<	>3<	>4<	><	/</pre>

* *in.c*, *in.py* - generates a random number of array length, and populate that array by random numbers. [ Check output   `>`]
  <pre style="background: rgb(238, 238, 238); border: 1px solid rgb(204, 204, 204); padding: 5px 10px;">./inc
  ./inc > in.txt</pre>

* *double.c* - Enter a number and those number of ints, ouputs double of them . [ Meant for testing piping] -
 <pre style="background: rgb(238, 238, 238); border: 1px solid rgb(204, 204, 204); padding: 5px 10px;">./d
 ./d < in.txt > double.txt
 ./inc | ./d</pre>

* *tertiary.c* - Enter a number and an array of that length. Outputs 10x array [ Meant for testing 3 pipes]
 <pre style="background: rgb(238, 238, 238); border: 1px solid rgb(204, 204, 204); padding: 5px 10px;">./inc | ./t
./inc | ./d | ./t</pre>

* *notif.c* - Displays hello world messages for some time. [For background execution]
<pre style="background: rgb(238, 238, 238); border: 1px solid rgb(204, 204, 204); padding: 5px 10px;">./notif
./notif &</pre>

* *netcheck* - Check your ping, again for background execution checking
<pre style="background: rgb(238, 238, 238); border: 1px solid rgb(204, 204, 204); padding: 5px 10px;">python netcheck
python netcheck &</pre>

Running in background returns the process of pid , which can be stopped by `kill -9 PID`
