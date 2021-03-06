/* Expressions
 */
EXPRLIT("x")
EXPRLIT("1-x")
EXPRLIT("sin(x*PI)")
EXPRLIT("1-sin(x*PI)")

EXPRLIT("sin(1*x*PI+PI/2)/2+.5")
EXPRLIT("sin(1*x*PI-PI/2)/2+.5")
EXPRLIT("sin(2*x*PI+PI/2)/2+.5")
EXPRLIT("sin(2*x*PI-PI/2)/2+.5")
EXPRLIT("sin(3*x*PI+PI/2)/2+.5")
EXPRLIT("sin(3*x*PI-PI/2)/2+.5")
EXPRLIT("sin(4*x*PI+PI/2)/2+.5")
EXPRLIT("sin(4*x*PI-PI/2)/2+.5")
EXPRLIT("sin(5*x*PI+PI/2)/2+.5")
EXPRLIT("sin(5*x*PI-PI/2)/2+.5")

EXPRLIT("triwave(x)")
EXPRLIT("triwave(x+.5)")
EXPRLIT("triwave(x*1.5)")
EXPRLIT("triwave(x*1.5+.5)")
EXPRLIT("triwave(x*2)")
EXPRLIT("triwave(x*2+.5)")
EXPRLIT("triwave(x*2.5)")
EXPRLIT("triwave(x*2.5+.5)")

EXPRLIT("sin(x*PI/2)")
EXPRLIT("1-sin((1-(x-1))*PI/2)")
EXPRLIT("-cos(x*PI/2)+1")
EXPRLIT("-cos((1-(x-1))*PI/2)")

EXPRLIT("log(x+1,2)")
EXPRLIT("log(2*x+1,3)")
EXPRLIT("log(4*x+1,5)")
EXPRLIT("log(8*x+1,9)")

EXPRLIT("pow(x,1/3)")
EXPRLIT("pow(x,1/2)")
EXPRLIT("pow(x,2/3)")
EXPRLIT("pow(x,1.5)")
EXPRLIT("pow(x,2)")
EXPRLIT("pow(x,2.5)")
EXPRLIT("pow(x,3)")
EXPRLIT("pow(x,3.5)")
EXPRLIT("pow(x,4)")

EXPRLIT("((x*255)&(x*255<<1))/255")
EXPRLIT("((x*255)|(x*255>>1))/255")
EXPRLIT("((x*255)^(x*255>>1))/255")

EXPRLIT("((sin(x*(4*PI)) + sin(x*(2*PI)))/4)+.5")
EXPRLIT("1-(((sin(x*(4*PI)) + sin(x*(2*PI)))/4)+.5)")
EXPRLIT("((sin(x*(8*PI)) + sin(x*(4*PI)) + sin(x*(4*PI)))/5)+.5")
EXPRLIT("1-(((sin(x*(8*PI)) + sin(x*(4*PI)) + sin(x*(4*PI)))/5)+.5)")

/* LViewP1b expressions
EXPRLIT("pow(x,1.5)")
EXPRLIT("pow(x,(1/2))")
EXPRLIT("pow(x,(2/3))")
EXPRLIT("log(1+x,2)")
EXPRLIT("log(1+x,10)")
EXPRLIT("x<.8?x:.8+(x-.8)/2")
EXPRLIT("x*.7+.15")
EXPRLIT("x/.7-3/14")
EXPRLIT("log(9*x+1,10)")
EXPRLIT("log(4*x+1,5)")
EXPRLIT("x<=.5?sin(x*PI)/2:(sin(x*PI+PI)+1)/2+.5")
*/