#define DOGPIN D5
#define CATPIN D4

class States
{
    private:
        String* State;
        int Length;
        bool* Status; //same length
        int* Pin;
    public:
        States(String State[], int Length, int Pin[])
        {
            this->State = State;
            this->Length = Length;
            this->Pin = Pin;
            this->Status = new bool[Length];
            for (int i = 0; i < Length; i++)
            {
                Status[i] = false;
            }
        }
        void Toggle(String State)
        {
            for (int i = 0; i < Length; i++)
            {
                if (this->State[i] == State)
                {
                    if (Status[i]) // is on
                    {
                        Status[i] = false; //turn state off
                        digitalWrite(Pin[i], LOW);
                    }
                    else
                    {
                        Status[i] = true; // turn state on
                        digitalWrite(Pin[i], HIGH);
                    }
                }
            }
        }
        void SwitchOff()
        {
            for (int i = 0; i < Length; i++)
            {
                Status[i] = false; //turn state off
                digitalWrite(Pin[i], LOW);
            }
        }
};

String S[] = {"dog", "cat"};
int C[] = {DOGPIN, CATPIN};
States states(S, 2, C);

void setup()
{
    pinMode(DOGPIN, OUTPUT);
    pinMode(CATPIN, OUTPUT);
    
    Particle.function("door", StateToggler);
    delay(1000);
}

void loop()
{

}

int StateToggler(String state)
{
    if (state == "off")
    {
        states.SwitchOff();
    }
    else
    {
        states.Toggle(state);
    }
    return 1;
}