%name valParser

%header{
/**********************************************************************/
/*	INSTITUTO DE CIBERNETICA 				      */
/*	INTERPRETE DE VAL-II	-	Analizador Sintactico         */
/*	Autor: Jorge Villalobos// Leopold Palomo		      */
/* Aportacions inconmensurables d'Antoni Soto */
/*	Fecha: Julio 1990/ Abril 2003					      */
/**********************************************************************/
/**********************************************************************/
/*			INCLUDE DE FICHEROS			      */
/**********************************************************************/
#include <iostream>
//#include <string>
#include <fstream>
#include <sstream>
#include <FlexLexer.h>

#include "interpret/ppoint.h"
#include "interpret/point.h"
#include "simulator/rhmatrix.h"
#include "simulator/rhrteuler.h"
#include "simulator/rchain.h"
#include "interpret/list_point.h"
#include "interpret/list_ppoint.h"
#include <functions.h>

struct typeitemvar{
   int tipus;
   Rhmatrix *rh;
   double *q;
};

struct typeppoint{
   int lon;
   double *value;
};

struct typevar{
   int tipus;
   string *name;
};



#include <qstring.h>

using namespace std;

%}

%union
{
  int y_int;
  std::string *y_str;
  double y_double;
  typeitemvar y_typeitemvar;
  typeppoint y_typeppoint;
  typevar y_typevar;
}

/*=============Parser class members=============*/
%define MEMBERS \
   virtual ~valParser() {}                      \
   Rchain *kinechain;                           \
   void setKineckain (Rchain * pkinechain);     \
   list_point l_point;                          \
   list_ppoint l_ppoint;                        \
   int parse(ifstream* in);                     \
   int parse(istringstream* in);                \
   int parse();                                 \
   int errorLine();                             \
   int errorColumn();                           \
   virtual int yyerror(std::string);                 \
   virtual void message(std::string);                \
   int dof;                                     \
   yyFlexLexer valLexer;
                                                                      
/*========= Constructor params + code ==========*/
%define CONSTRUCTOR_PARAM Rchain *pkinechain
%define CONSTRUCTOR_CODE{kinechain = pkinechain; \
                         dof = kinechain->dof;   \
                         l_ppoint.init(dof);}

/*===== The error reporting funtion body =======*/
%define ERROR_BODY { }

/*==============Lexer body call=================*/
%define LEX_BODY { return valLexer.yylex(); }


/**********************************************************************/
/*			COMANDOS AL INTERPRETE			      */
/**********************************************************************/

%token TkDELETEL
%token TkDELETEP
%token TkLISTL
%token TkLOAD
%token TkSTOREL
%token TkEXE
%token TkDIR
%token TkPCEXECUTE
%token TkPCEND
%token TkTRACE
%token TkUNTRACE

/**********************************************************************/
/*			TOKENS DEL LENGUAJE			      */
/**********************************************************************/

%token TkPROGRAM
%token TkLOCATIONS
%token TkREALS
%token TkMAS
%token TkMENOS
%token TkPOR
%token TkDIVISION
%token TkMODULO
%token TkEQ
%token TkNE
%token TkLT
%token TkGT
%token TkLE
%token TkGE
%token TkNOT
%token TkAND
%token TkOR
%token TkTRUE
%token TkFALSE
%token TkDEPART
%token TkDEPARTS
%token TkAPPRO
%token TkAPPROS
%token TkMOVE
%token TkMOVES
%token TkCLOSEI
%token TkOPENI
%token TkSET
%token TkDELAY
%token TkWHERE
%token TkSPEED
%token TkHERE
%token TkWORLD
%token TkTRANS
%token TkPPOINT
%token TkINRANGE
%token TkINVERSE
%token TkIF
%token TkTHEN
%token TkELSE
%token TkEND
%token TkWHILE
%token TkDO
%token TkUNTIL
%token TkFOR
%token TkTO
%token TkSTEP
%token TkCASE
%token TkOF
%token <y_typevar> TkVARP
%token <y_typevar> TkVARPP
%token <y_double> TkNUMERO
%token <y_str> TkSTRING
%token TkCALL
%token TkRETURN
%token TkTYPE
%token TkPROMPT

/**********************************************************************/
/*			    EXTENSIONES AL LENGUAJE	              */
/**********************************************************************/

%token TkBODY
%token TkPLACE
%token TkSPLACE
%token TkFLIPVIEW
%token TkVIEW
%token TkDRAWLOC
%token TkDRAWLOCS
%token TkERASELOC
%token TkERASELOCS
%token TkALIGN
%token TkREADY

/**********************************************************************/
/*			    CODIGO INTERMEDIO		              */
/**********************************************************************/

%token TkMULTVAR
%token TkNIF
%token TkGOTO
%token TkASIGNE
%token TkPUSH
%token TkPUSHTXT
%token TkPUSHK
%token TkPUSHVAR
%token TkLOOKFOR
%token TkPUTNAME
%token TkPOP
%token TkSUPEQ
%token TkTYPESTR

/**********************************************************************/
/*			NO-TERMINALES CON RETORNO		      */
/**********************************************************************/

/* %type <y_str> expr.variable */
%type <y_typevar> variable
%type <y_typeppoint> lexpr.arit
%type <y_typeitemvar> item.der.var 
%type <y_double> expr.arit

%start instruccion

%%

instruccion:
      TkDELAY   expr.arit
      {/* gen(TkDELAY,0); */}

   |  TkMOVE    item.der.var
			{
            if ($2.tipus == 0) //
            {
               kinechain->move(*$2.rh);
               delete   $2.rh;
            }
            else
            {
               kinechain->move($2.q);
               delete   $2.q;
            }
         }

	|	TkMOVES   item.der.var
			{ }

	|	TkWHERE
			{kinechain->tcp();}

	|	TkSPEED  expr.arit
			{/*parse->kinechain*//* gen(TkSPEED,0); */}

	|	TkHERE  variable
			{
            if ($2.tipus == 0) //
            {
                point p; p.name = $2.name->c_str();
                p.value = kinechain->mat_tcp();
                l_point.update(p);
                delete $2.name;
            }
			/* gen(TkHERE,$2); */}
      
	|	TkSET  TkWORLD  '='  item.der.var
			{      
            if ($4.tipus == 0) //
            {
            	point p; 
               p.value = *$4.rh;
               kinechain->settranform_Tworld(p.value);
               delete $4.rh;
                  
            }
            else
            {
            	message("You must define a point not a precision point");
						
               delete $4.q;
            }
            
         }
			
			  		
	|	TkSET  variable  '='  item.der.var
			{  if ($2.tipus == $4.tipus)
            {
               if ($2.tipus == 0) //
               {
                  point p; p.name = $2.name->c_str();
                  p.value = *$4.rh;
                  l_point.update(p);
                  delete $4.rh;
                  delete $2.name;
               }
               else
               {
                  ppoint p(dof);

                  p.name = $2.name->c_str();

                  for(int i= 0;i<dof;i++)
                     p.q[i] = $4.q[i];

                  l_ppoint.update(p);
                  delete $4.q;
                  delete $2.name;
               }
            }
            else
            {  message("Type mismatch betwend vars");
               if ($2.tipus == 0) //
               {
                  delete   $4.rh;
                  delete   $2.name;
               }
               else
               {
                  delete   $4.q;
                  delete   $2.name;
               }
               return(true);
            }
         }
                           
	|	TkDRAWLOCS
			{/* gen(TkDRAWLOCS,0); */}

	|	TkERASELOC  variable
			{/* gen(TkERASELOC,0); */}

	|	TkDELETEL  variable  
			{ /*if($3== -1)
			    delete_loc(getNom($2));
			  else
			    delete_loc(newName($2));
			*/}

	|	TkREADY
			{kinechain->do_ready(); }
	;


  
item.der.var:	TkTRANS  '('  expr.arit  ','
				 expr.arit  ','
				 expr.arit  ','
				 expr.arit  ','
				 expr.arit  ','
				 expr.arit
		          ')'
			{ $$.rh = new Rhmatrix; Rhrteuler trans;
           *$$.rh = trans.tr_ZYZ_XYZ ($3, $5, $7, rad($9), rad($11), rad($13));
           $$.tipus = 0;}

		|	TkPPOINT '('  lexpr.arit ')'
		   {
            if ($3.lon == dof)
            {
               $$.tipus = 1;
               $$.q = $3.value;
            }       
            else{
               message("You have to define dof parameters");
               delete $3.value;
               return (true);
            }
         }

      |	variable
			{  if($1.tipus == 0)
            {
               $$.tipus = 0;
               point p = l_point.manage(QString::QString($1.name->c_str()));

               delete $1.name;
               if (!p.name.isEmpty())
               {
                  $$.rh = new Rhmatrix;
                  *$$.rh = p.value;
               }
               else
               {
                  message("Idiot, define the var first ");
                  return (true);
               }

            }
            else
            {
               $$.tipus =  1;
               ppoint p = l_ppoint.manage(QString::QString($1.name->c_str()));
               delete $1.name;
               if (!p.name.isEmpty())
               {
                  $$.q = new double[dof];
                  for (int i = 0; i < dof; i++)
                     $$.q[i] = p.q[i];
               }
               else
               {
                  message("Idiot, define the var first");
                  return (true);
               }
            }
         }
	;

variable:	TkVARP
			{ $$ = $1; }
	|	TkVARPP
			{ $$ = $1; }
	;


expr.arit:	  TkNUMERO
			{$$ = $1; }

	;
lexpr.arit:    expr.arit
         {$$.lon = 1;
          $$.value = new double[dof];
          if(kinechain->dh_parameters[1].sigma == 0)
            $$.value[0] = $1;
          else
            $$.value[0] = rad($1);
          }

   |     lexpr.arit ',' expr.arit
         {  $$.lon = $1.lon + 1;
            $$.value = $1.value;

            if ($$.lon <= dof)
            {
               if(kinechain->dh_parameters[$1.lon].sigma == 0)
                  $$.value[$1.lon] = $3;
               else
                  $$.value[$1.lon] = rad($3);

            }
            else
            {
               message("You have exceed the parameters");
               delete $$.value;
               return (true);
            }
         }
   ;
%%


/* -------------- body section -------------- */
// feel free to add your own C/C++ code here

extern void setvalParser(valParser*);

int valParser::parse(ifstream* in)
{
  //set new input stream for lexer
  setvalParser(this);
  valLexer.switch_streams(in,0);
  bool ok=1-yyparse();
  return ( ok );
}

int valParser::parse(istringstream* in)
{
  //set new input stream for lexer
  setvalParser(this);
  valLexer.switch_streams(in,0);
  bool ok=1-yyparse();
  return ( ok );
}

int valParser::parse(){

  //this defaults to cin as input stream.
  setvalParser(this);
  valLexer.switch_streams(0,0);
  bool ok=1-yyparse();
  return (ok );
}

int valParser::yyerror( string s ) {
  cerr << "SYNTAX ERROR at line " /* << getvalLineNr() */<< s <<endl;
  YYACCEPT;
  return 0;
}

void valParser::message(string s) {
  cout << s <<endl;
}                 
