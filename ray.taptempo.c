#include "ext.h"							// standard Max include, always required
#include "ext_obex.h"						// required for new style Max object

typedef struct
{
  t_object ob;			// the object itself (must be first)

  long m_unit;
  long m_beat;

  void *m_out1;
} TapTempo;

void *taptempo_new(t_symbol *s, long argc, t_atom *argv);

void taptempo_free  (TapTempo *x);
void taptempo_assist(TapTempo *x, void *b, long m, long a, char *s);
void taptempo_bang  (TapTempo *x);

//////////////////////// global class pointer variable
void *taptempo_class;


int C74_EXPORT main(void){

  t_class *c;
  c = class_new("ray.taptempo", (method)taptempo_new, (method)taptempo_free, (long)sizeof(TapTempo), 
      0L /* leave NULL!! */, A_GIMME, 0);

  class_addmethod(c, (method)taptempo_assist,			"assist",		A_CANT, 0);  
  class_addmethod(c, (method)taptempo_bang, "bang", 0);

  class_register(CLASS_BOX, c);
  taptempo_class = c;

  return 0;
}

void *taptempo_new(t_symbol *s, long argc, t_atom *argv)
{
  TapTempo *x = NULL;

  // object instantiation, NEW STYLE
  x = (TapTempo *)object_alloc(taptempo_class);
  if (x) {

    if(argc >= 2 && argv[0].a_type == A_LONG && argv[1].a_type == A_LONG){

      x -> m_beat = atom_getlong(&argv[0]);
      x -> m_unit = atom_getlong(&argv[1]);
    }
    else{
      post("First Argument:  (int)beat");
      post("Second Argument: (int)unit");
      post("default: unit : beat = 4 : 4");
      x -> m_unit = 4;
      x -> m_beat = 4;
    }
    
    x -> m_out1 = floatout((TapTempo *)x);
  }
  return (x);
}



void taptempo_assist(TapTempo *x, void *b, long m, long a, char *s)
{
  if (m == ASSIST_INLET)
    sprintf(s, "(bang)Tap in");
  else
    sprintf(s, "(float)Average BPM"); 			
}



void taptempo_free(TapTempo *x){}



void taptempo_bang(TapTempo *x){

  static double time  = 0;
  static double prev  = 0;
  static int    count = 0;
  static double now   = 0;


  clock_getftime(&now);
  if(count == 0) prev = now;

  double diff = now - prev;
  if(diff > 5000){
    count = 0;
    prev = now;
  }

  time += diff;
  prev = now;

  if(count >= x -> m_beat - 1){

    double bpm = 60000. / ( ( time / (count) ) * (x -> m_unit / 4.0) );

    
    outlet_float(x -> m_out1, bpm);

    count = 0;
    time  = 0;
    return;
  }

  count ++;
}

