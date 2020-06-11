
#define dirPin 47
#define stepPin 45
int state = 0;
int steps = 0;
int i = 1;
float divsion = 0;
float timeVal = 1000; //50ms
int startMax = 0;
int endMax = 0;
float feq = 0;
bool flag = false;
int value = 0;
bool timerFlag = false;
void setup() {
  // put your setup code here, to run once:
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;
  OCR1A  = 7;       // compare match register 16MHz/256/2Hz
  TCCR1B = TCCR1B | (1 << WGM12);  // CTC mode
  TCCR1B = TCCR1B | ((1 << CS12) | (1 << CS10)); // 256 prescaler
  TIMSK1 = TIMSK1 | (1 << OCIE1A); // enable timer compare interrup
  Serial.begin(9600);
  digitalWrite(dirPin, HIGH);
  Serial.println("############ Welcome to speed controlling application ############");
  Serial.println("type \"Step\" to start");
}



void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available() > 0)
  {
    String read_input = Serial.readString();
    read_input.trim();
    read_input.toUpperCase();
    value = read_input.toInt();
    if (read_input == "STEP")
    {
      flag = true;
      Serial.println("Enter Steps:");
    }
    if (flag && value>0)
    {
      i=0;
      steps = value;
      cal_range(steps);
      divsion = ((1000 - 12) / (float)startMax) * 2;
      flag = false;
      timerFlag = true;
    }
  }
}

ISR(TIMER1_COMPA_vect)          // timer compare interrupt service routine
{
  if (timerFlag)
  {
    if (i <= (steps * 2)) {
      if (i < startMax) {
        timeVal = timeVal + divsion;
        OCR1A = getOCRIAValue(timeVal);
      }
      else if (i >= (endMax * 2 + startMax) )
      {
        timeVal = timeVal - divsion;
        OCR1A = getOCRIAValue(timeVal);
      }
      state = !state;
      digitalWrite(stepPin, state);
      i++;
    }
    else
      timerFlag = false;
  }
}

void cal_range(int steps) {
  startMax = (steps * 20) / 100;
  endMax = startMax * 4;

}

int getOCRIAValue(float microSec )
{
  return ((16000000 / (microSec * 1024) - 1));
}
