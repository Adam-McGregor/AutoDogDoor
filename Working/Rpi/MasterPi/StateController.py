#import smbus
from smbus2 import SMBus
import time
import RPi.GPIO as GPIO

## break beam sensors ##
DOGPIN = 14
CATPIN = 15
GPIO.setmode(GPIO.BCM)
GPIO.setup(DOGPIN, GPIO.IN)
GPIO.setup(CATPIN, GPIO.IN)

ARGONDOGPIN = 18
ARGONCATPIN = 23

GPIO.setup(ARGONDOGPIN, GPIO.IN)
GPIO.setup(ARGONCATPIN, GPIO.IN)

bus = SMBus(1)
Arduino = 0x04 # address of arduino

## Communication ##
def writeNumber(number): #number = "###", send one digit at a time
    for digit in number:
        bus.write_byte(Arduino, int(digit))
        time.sleep(0.001)
    time.sleep(1)
    return

def TargetNumber(): #has the target been reached
    try:
        target = bool(bus.read_byte(Arduino))
    except OSError:
        time.sleep(0.001) #try again
        print("Commuication Error Tryin Again")
        TargetNumber()
    time.sleep(1)
    return target

## Timer ##
TIME_PASSED = 5 # seconds to wait

class Timer:
    time = 0
    def __init__(self): 
        self.Time = time.time()
    def CheckTime(self):
        if (time.time() - self.Time >= TIME_PASSED):
            print("Times Up")
            ResetState()
            return True
        return False
    def ResetTime(self):
        self.Time = time.time()

Time = Timer()

## State ##
class State:
    name = ""
    height = 0
    def __init__(self, name, height):
        self.name = name
        self.height = height

class StateController:
    state = [State("initial", "000"), State("dog", "700"), State("cat", "350")] ## legal states (initial = closed)
    CurrentState = state[0]
    AtTarget = False #whether to start the clock (false means reset clock when at target)
    def __init(self):
        self.CurrentState = self.state[0] # initial state
    def ChangeState(self, newState): #by name
        for i in range(len(self.state)):
            if (newState == self.state[i].name):
                self.CurrentState = self.state[i]
                writeNumber(str(self.GetState().height))
                time.sleep(1)
                return True
        return False
    def GetState(self):
        return self.CurrentState
    def GetStateHeight(self, state):
        for i in range(len(self.state)):
            if (state == self.state[i].name):
                return self.state[i].height
        return 0

## Change State(s) ##

Controller = StateController()
def ChangeState(newState):
    if (int(Controller.CurrentState.height) < int(Controller.GetStateHeight(newState))): # Change State if there is a higher height
        Controller.ChangeState(newState)
        print("Setting to " + newState + " State")
        print("Height: " + Controller.GetState().height + " - State: " + Controller.GetState().name)
        
    Controller.AtTarget = False # Reset Target -- to reset time

def ResetState(): #doesn't matter if it makes it (as long as an event has triggered another state)
    Controller.ChangeState("initial")
    print("Closing Door")
    
    
## Program ##

try:
    while True:
        if (not GPIO.input(DOGPIN)):
            ChangeState("dog")
        if (not GPIO.input(CATPIN)):
            ChangeState("cat")
        if (GPIO.input(ARGONDOGPIN)):
            ChangeState("dog")
        if (GPIO.input(ARGONCATPIN)):
            ChangeState("cat")
        time.sleep(0.1)
        if (not Controller.AtTarget):
            if (TargetNumber()): #at the target start the clock
                Time.ResetTime()
                Controller.AtTarget = True
        else:
            if (not Controller.CurrentState.name == "initial"):
                Time.CheckTime()
except KeyboardInterrupt:
    print("Program Finished")
    GPIO.cleanup()