/************************************************************************
 ************************************************************************
 Copyright (C) 2021 GRAME, Centre National de Creation Musicale
 
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation; either version 2.1 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 
 ************************************************************************
 ************************************************************************/

#ifndef LIBFAUSTBOX_H
#define LIBFAUSTBOX_H

#include <string>
#include <vector>

/**
 * Opaque types.
 */
class CTree;
typedef CTree* Box;
typedef std::vector<Box> tvec;

/**
 * Create global compilation context, has to be done first.
 */
extern "C" void createLibContext();

/**
 * Destroy global compilation context, has to be done last.
 */
extern "C" void destroyLibContext();

/**
 * Constant integer : for all t, x(t) = n.
 *
 * @param n - the integer
 *
 * @return the integer box.
 */

Box boxInt(int n);

/**
 * Constant real : for all t, x(t) = n.
 *
 * @param n - the float/double value (depends of -single or -double compilation parameter)
 *
 * @return the float/double box.
 */
Box boxReal(double n);

/**
 * The identity box, copy its input to its output.
 *
 * @return the identity box.
 */
Box boxWire();

/**
* The cut box, to "stop"/terminate a signal.
*
* @return the cut box.
*/
Box boxCut();

/**
 * The sequential composition of two blocks (e.g., A:B) expects: outputs(A)=inputs(B)
 *
 * @return the seq box.
 */
Box boxSeq(Box x, Box y);

/**
 * The parallel composition of two blocks (e.g., A,B).
 * It places the two block-diagrams one on top of the other, without connections.
 *
 * @return the par box.
 */
Box boxPar(Box x, Box y);

/**
 * The split composition (e.g., A<:B) operator is used to distribute
 * the outputs of A to the inputs of B.
 *
 * For the operation to be valid, the number of inputs of B
 * must be a multiple of the number of outputs of A: outputs(A).k=inputs(B)
 *
 * @return the split box.
 */
Box boxSplit(Box x, Box y);

/**
 * The merge composition (e.g., A:>B) is the dual of the split composition.
 *
 * The number of outputs of A must be a multiple of the number of inputs of B: outputs(A)=k.inputs(B)
 *
 * @return the merge box.
 */
Box boxMerge(Box x, Box y);

/**
 * The recursive composition (e.g., A~B) is used to create cycles in the block-diagram
 * in order to express recursive computations.
 * It is the most complex operation in terms of connections: outputs(A)≥inputs(B) and inputs(A)≥outputs(B)
 *
 * @return the rec box.
 */
Box boxRec(Box x, Box y);

/**
 * The route primitive facilitates the routing of signals in Faust.
 * It has the following syntax: route(A,B,a,b,c,d,...) or route(A,B,(a,b),(c,d),...)
 *
 * @param n -  the number of input signals
 * @param m -  the number of output signals
 * @param r - the routing description, a 'par' expression of a,b / (a,b) input/output pairs

 * @return the route box.
 */

Box boxRoute(Box n, Box m, Box r);

/**
 * Create a delayed box.
 *
 * @return the delayed box.
 */
Box boxDelay();

/**
 * Create a casted box.
 *
 * @return the casted box.
 */
Box boxIntCast();

/**
 * Create a casted box.
 *
 * @return the casted box.
 */
Box boxFloatCast();

/**
 * Create a read only table.
 *
 * @return the table box.
 */
Box boxReadOnlyTable();

/**
 * Create a read/write table.
 *
 * @return the table box.
 */
Box boxWriteReadTable();

/**
 * Create a waveform.
 *
 * @param wf - the content of the waveform as a vector of boxInt or boxDouble boxes
 *
 * @return the waveform box.
 */
Box boxWaveform(const tvec& wf);

/**
 * Create a soundfile block.
 *
 * @param label - of form "label[url:{'path1';'path2';'path3'}]" to describe a list of soundfiles
 * @param chan - the number of outputs channels
 *
 * @return the soundfile box.
 */
Box boxSoundfile(const std::string& label, Box chan);

/**
 * Create a selector between two boxes.
 *
 * @return the selected box depending of the selector value at each time t.
 */
Box boxSelect2();

/**
 * Create a selector between three boxes.
 *
 * @return the selected box depending of the selector value at each time t.
 */
Box boxSelect3();

enum SType { kSInt, kSReal };

/**
 * Create a foreign constant box.
 *
 * @param type - the foreign constant type of SType
 * @param name - the foreign constant name
 * @param file - the include file where the foreign constant is defined
 *
 * @return the foreign constant box.
 */
Box boxFConst(SType type, const std::string& name, const std::string& file);

/**
 * Create a foreign variable box.
 *
 * @param type - the foreign variable type of SType
 * @param name - the foreign variable name
 * @param file - the include file where the foreign variable is defined
 *
 * @return the foreign variable box.
 */
Box boxFVar(SType type, const std::string& name, const std::string& file);

enum SOperator { kAdd, kSub, kMul, kDiv, kRem, kLsh, kARsh, kLRsh, kGT, kLT, kGE, kLE, kEQ, kNE, kAND, kOR, kXOR };

/**
 * Generic binary mathematical functions.
 *
 * @param op - the operator in SOperator set
 *
 * @return the result box of op(x,y).
 */
Box boxBinOp(SOperator op);

/**
 * Specific binary mathematical functions.
 *
 * @return the result box.
 */
Box boxAdd();
Box boxSub();
Box boxMul();
Box boxDiv();
Box boxRem();

Box boxLeftShift();
Box boxLRightShift();
Box boxARightShift();

Box boxGT();
Box boxLT();
Box boxGE();
Box boxLE();
Box boxEQ();
Box boxNE();

Box boxAND();
Box boxOR();
Box boxXOR();

/**
 * Extended unary mathematical functions.
 */

Box boxAbs();
Box boxAcos();
Box boxTan();
Box boxSqrt();
Box boxSin();
Box boxRint();
Box boxLog();
Box boxLog10();
Box boxFloor();
Box boxExp();
Box boxExp10();
Box boxCos();
Box boxCeil();
Box boxAtan();
Box boxAsin();

/**
 * Extended binary mathematical functions.
 */

Box boxRemainder();
Box boxPow();
Box boxMin();
Box boxMax();
Box boxFmod();
Box boxAtan2();

/**
 * Create a button box.
 *
 * @param label - the label definition (see [2])
 *
 * @return the button box.
 */
Box boxButton(const std::string& label);

/**
 * Create a checkbox box.
 *
 * @param label - the label definition (see [2])
 *
 * @return the checkbox box.
 */
Box boxCheckbox(const std::string& label);

/**
 * Create a vertical slider box.
 *
 * @param label - the label definition (see [2])
 * @param init - the init box, a constant numerical expression (see [1])
 * @param min - the max box, a constant numerical expression (see [1])
 * @param max - the min box, a constant numerical expression (see [1])
 * @param step - the step box, a constant numerical expression (see [1])
 *
 * @return the vertical slider box.
 */
Box boxVSlider(const std::string& label, Box init, Box min, Box max, Box step);

/**
 * Create an horizontal slider box.
 *
 * @param label - the label definition (see [2])
 * @param init - the init box, a constant numerical expression (see [1])
 * @param min - the max box, a constant numerical expression (see [1])
 * @param max - the min box, a constant numerical expression (see [1])
 * @param step - the step box, a constant numerical expression (see [1])
 *
 * @return the horizontal slider box.
 */
Box boxHSlider(const std::string& label, Box init, Box min, Box max, Box step);

/**
 * Create a num entry box.
 *
 * @param label - the label definition (see [2])
 * @param init - the init box, a constant numerical expression (see [1])
 * @param min - the max box, a constant numerical expression (see [1])
 * @param max - the min box, a constant numerical expression (see [1])
 * @param step - the step box, a constant numerical expression (see [1])
 *
 * @return the num entry box.
 */
Box boxNumEntry(const std::string& label, Box init, Box min, Box max, Box step);

/**
 * Create a vertical bargraph box.
 *
 * @param label - the label definition (see [2])
 * @param min - the max box, a constant numerical expression (see [1])
 * @param max - the min box, a constant numerical expression (see [1])
 *
 * @return the vertical bargraph box.
 */
Box boxVBargraph(const std::string& label, Box min, Box max);

/**
 * Create an horizontal bargraph box.
 *
 * @param label - the label definition (see [2])
 * @param min - the max box, a constant numerical expression (see [1])
 * @param max - the min box, a constant numerical expression (see [1])
 *
 * @return the horizontal bargraph box.
 */
Box boxHBargraph(const std::string& label, Box min, Box max);

/**
 * Create an attach box.
 *
 * The attach primitive takes two input boxes and produces one output boxes
 * which is a copy of the first input. The role of attach is to force
 * its second input boxes to be compiled with the first one.
 *
 * @return the attach box.
 */
Box boxAttach();

/**
 * Base class for factories.
 */
struct dsp_factory_base {
    
    virtual ~dsp_factory_base() {}
    
    virtual void write(std::ostream* out, bool binary = false, bool compact = false) {}
};

/**
 * Compile a box expression in a list of signals
 *
 * @param box - the box expression
 * @param error_msg - the error string to be filled
 *
 * @return a list of signals on success, otherwise an empty list.
 */
tvec boxesToSignals(Box box, std::string& error_msg);

/**
 * Create a C++ Faust DSP factory from a box expression.
 *
 * @param name_app - the name of the Faust program
 * @param box - the box expression
 * @param argc - the number of parameters in argv array
 * @param argv - the array of parameters
 * @param error_msg - the error string to be filled
 *
 * @return a DSP factory on success, otherwise a null pointer.
 */
dsp_factory_base* createCPPDSPFactoryFromBoxes(const std::string& name_app, Box box,
                                               int argc, const char* argv[],
                                               std::string& error_msg);

/*
 [1] Constant numerical expression : see https://faustdoc.grame.fr/manual/syntax/#constant-numerical-expressions
 [2] Label definition : https://faustdoc.grame.fr/manual/syntax/#variable-parts-of-a-label
 */

#endif