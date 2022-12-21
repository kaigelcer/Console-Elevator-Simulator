# Console-Elevator-Simulator
<p>
                            This project was completed as an assignment 
                            for my Systems Software Engineering course.
                             The objective was to design a dispatcher 
                             algorithm for a duplex elevator system 
                             along with a visual simulation model using 
                             multithreading in C++. Later on, I updated 
                             the system to work with up to 10 elevators.
                        </p>
                        <p>
                            In this model, there is a single process 
                            representing the entire elevator system, 
                            with multiple threads each representing 
                            one elevator. The system responds to a 
                            set of user inputs which simulate real-world 
                            requests and scenarios. Users can also enable a
                            feature called "Dynamic Passengers" which
                            activates a new thread that periodically sends 
                            requests to each elevator, simulating passengers. 
                            A passenger will request to go up / down from 
                            a certain floor, then enter their destination 
                            floor when an elevator picks them up.  See the 
                            table below for the list of valid inputs :
                        </p>
<table>
                            <tr>
                                <th>First Input</th>
                                <th>Second Input</th>
                                <th>Result</th>
                            </tr>
                            <tr>
                                <td>Input is a number x, 0 ≤ x ≤ 9</td>
                                <td rowspan=5 id="multirow_cell">Input is a number y, 0 ≤ y ≤ 9</td>
                                <td>From inside of elevator x, a passenger requests travel to floor y</td>
                            </tr>
                            <tr>
                                <td>‘u’</td>
                                <td>From floor y, a passenger wishes to travel up</td>
                            </tr>
                            <tr>
                                <td>‘d’</td>
                                <td>From floor y, a passenger wishes to travel down</td>
                            </tr>
                            <tr>
                                <td>‘-’</td>
                                <td>Simulates a fault, elevator y is out of service</td>
                            </tr>
                            <tr>
                                <td>‘+’</td>
                                <td>Fixes a fault, elevator y is now in service</td>
                            </tr>
                            <tr>
                                <td rowspan=2 id="multirow_cell">‘d’</td>
                                <td id="multirow_cell">'+'</td>
                                <td>Enable dynamic passengers</td>
                            </tr>
                            <tr>
                                <td>'-'</td>
                                <td>Disable dynamic passengers</td>
                            </tr>
                            <tr>
                                <td>'e'</td>
                                <td id="multirow_cell">'e'</td>
                                <td>Halt simulation, both elevators 
                                    return to ground floor and stop</td>
                            </tr>
                        </table>

<h2>The Algorithm</h2>
The algorithm, in pseudo-code, is roughly as follows:

- ON up/down request received:
  - IF any elevator has an empty queue:
    - Send the request to that elevator
  - ELSE IF any elevator is passing by the request AND the request direction is the same as the elevators current direction:
    - Send the request to that elevator
  - ELSE:
    - Send request to elevator with the shortest queue

<p>
                            In theory it could be made more efficient
                            (perhaps by inserting requests into specific
                            spots within each elevator’s queue), but for
                            a realistic amount of requests over time, 
                            this algorithm does the trick. Considering
                            how long I wait for the elevators in my 
                            apartment building, I reckon my algorithm
                            is up to industry standard. 
</p>
