/***************************************************************************
 *   Copyright (C) 2009 by BUI Quang Minh   *
 *   minh.bui@univie.ac.at   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

/*
	collection of classes for Next-generation sequencing 
*/

#include "ngs.h"

/****************************************************************************
        NGSAlignment
 ****************************************************************************/

NGSAlignment::NGSAlignment(const char *filename) : AlignmentPairwise() {
	readFritzFile(filename);
}

void NGSAlignment::readFritzFile(const char *filename) {
	cout << "Reading Fritz file " << filename << " ..." << endl;
	try {
		ifstream in;
		in.exceptions(ios::failbit | ios::badbit);
		in.open(filename);
		in.clear();
		int i, total_size;
		string tmp;
		in >> tmp;
		ncategory = convert_int(tmp.c_str());
		if (ncategory < 1) throw "Wrong number of positions";
		in >> tmp;
		num_states = convert_int(tmp.c_str());
		total_size = ncategory*num_states*num_states;
		if (num_states < 1) throw "Wrong number of states";
		pair_freq = new int[total_size];
		for (i=0; i < total_size; i++) {
			in >> tmp;
			int count = convert_int(tmp.c_str());
			if (count < 0) throw "Wrong count";
			pair_freq[i] = count;
		}
		// set the failbit again
		in.exceptions(ios::failbit | ios::badbit);
		in.close();
	} catch (const char *str) {
		outError(str);
	} catch (string str) {
		outError(str);
	} catch(ios::failure) {
		outError(ERR_READ_INPUT);
	}

	cout << ncategory << " matrices of size " << num_states << endl;
}

void NGSAlignment::computeStateFreq (double *stateFrqArr) {
	int cat, i, j, id = 0;
	int state_count[num_states];
	memset(state_count, 0, sizeof(int)*num_states);
	for (cat = 0, id = 0; cat < ncategory; cat++) {
		for (i = 0; i < num_states; i++)
			for (j = 0; j < num_states; j++, id++) {
				state_count[i] += pair_freq[id];
				state_count[j] += pair_freq[id];
			}
	}

	int sum_count = 0;
	for (i = 0; i < num_states; i++) sum_count += state_count[i];
	for (i = 0; i < num_states; i++) stateFrqArr[i] = double(state_count[i]) / sum_count;
	cout << "Empirical state frequencies: ";
	for (i = 0; i < num_states; i++) 
		cout << stateFrqArr[i] << " ";
	cout << endl;
}


void NGSAlignment::computeEmpiricalRate (double *rates) {
	int i, j, k, cat, id;
	assert(rates);
	double **pair_rates = (double**) new double[num_states];
	for (i = 0; i < num_states; i++) {
		pair_rates[i] = new double[num_states];
		memset(pair_rates[i], 0, sizeof(double)*num_states);
	}

	for (cat = 0, id = 0; cat < ncategory; cat++) {
		for (i = 0; i < num_states; i++)
			for (j = 0; j < num_states; j++, id++) {
				pair_rates[i][j] += pair_freq[id];
			}
	}

	k = 0;
	double last_rate = pair_rates[num_states-2][num_states-1] + pair_rates[num_states-1][num_states-2];
	for (i = 0; i < num_states-1; i++)
		for (j = i+1; j < num_states; j++)
			rates[k++] = (pair_rates[i][j] + pair_rates[j][i]) / last_rate;
	cout << "Empirical rates: ";
	for (k = 0; k < num_states*(num_states-1)/2; k++)
		cout << rates[k] << " ";
	cout << endl;

	for (i = num_states-1; i >= 0; i--) {
		delete [] pair_rates[i];
	}
	delete [] pair_rates;
}

double NGSAlignment::computeEmpiricalDist(int cat) {
	int i;
	int trans_size = num_states*num_states;
	int *pair_pos = pair_freq + (cat*trans_size);
	int match_pos = 0, total_pos = 0;
	for (i = 0; i < num_states; i++) 
		match_pos += pair_pos[i*num_states+i];
	for (i = 0; i < trans_size; i++) 
		total_pos += pair_pos[i];
	if (total_pos == 0) total_pos = 1;
	return (double)(total_pos - match_pos) / total_pos;
}


double NGSAlignment::computeFunctionCat(int cat, double value) {
	int trans_size = num_states*num_states;
	double lh = 0.0;
	double trans_mat[trans_size];
	int i;

	tree->getModelFactory()->computeTransMatrix(value, trans_mat);
	int *pair_pos = pair_freq + cat*trans_size;

	for (i = 0; i < trans_size; i++) 
		lh -= pair_pos[i] * log(trans_mat[i]);
	return lh;
}


double NGSAlignment::computeFuncDervCat(int cat, double value, double &df, double &ddf) {
	int trans_size = num_states*num_states;
	double lh = 0.0;
	df = 0.0; ddf = 0.0;
	int i;
	double derv1 = 0.0, derv2 = 0.0;
	double trans_mat[trans_size], trans_derv1[trans_size], trans_derv2[trans_size];
	

	tree->getModelFactory()->computeTransDerv(value, trans_mat, trans_derv1, trans_derv2);
	int *pair_pos = pair_freq + cat*trans_size;
	for (i = 0; i < trans_size; i++) if (pair_pos[i] > 0) {
		double d1 = trans_derv1[i] / trans_mat[i];
		derv1 += pair_pos[i] * d1;
		derv2 += pair_pos[i] * (trans_derv2[i]/trans_mat[i] - d1 * d1);
		lh -= pair_pos[i] * log(trans_mat[i]);
	}
	//df -= derv1 * rate_val;
	//ddf -= derv2 * rate_val * rate_val;
	df -= derv1;
	ddf -= derv2;
	return lh;
}

/*double NGSAlignment::computeFunction(double value) {
	double lh = 0.0;
	for (int cat = 0; cat < ncategory; cat++)
		lh += computeFunctionCat(cat, value*tree->getRate()->getRate(cat));
	double lh_old = AlignmentPairwise::computeFunction(value);
	assert(fabs(lh-lh_old) < 0.001);
	return lh;
}

double NGSAlignment::computeFuncDerv(double value, double &df, double &ddf) {
	double lh = 0.0;
	df = ddf = 0.0;
	for (int cat = 0; cat < ncategory; cat++) {
		double derv1, derv2;
		lh += computeFuncDervCat(cat, value, derv1, derv2);
		df += derv1;
		ddf += derv2;
	}
	double df_old, ddf_old;
	double lh_old = AlignmentPairwise::computeFuncDerv(1.0, df_old, ddf_old);
	return lh;
}*/

/****************************************************************************
        NGSRate
 ****************************************************************************/
NGSRate::NGSRate(PhyloTree *tree) {
	phylo_tree = tree;
	ncategory = ((NGSAlignment*)tree->aln)->ncategory;
	rates = new double[ncategory];
	int i;
	for (i = 0; i < ncategory; i++) {
		rates[i] = ((NGSAlignment*)tree->aln)->computeEmpiricalDist(i);
		if (rates[i] < 1e-6) rates[i] = 1e-6;
	}
	
	name = "+F";
	name += convertIntToString(ncategory);
	full_name = name;
	is_categorized = true;

}

double NGSRate::optimizeParameters() {
	int cat;
	double negative_lh;
	for (cat = 0; cat < ncategory; cat++) {
		optimizing_cat = cat;
		rates[cat] = minimizeNewtonSafeMode(1e-6, rates[cat], MAX_GENETIC_DIST, 1e-6, negative_lh);
	}
	return phylo_tree->computeLikelihood();
}


double NGSRate::computeFunction(double value) {
	return ((NGSAlignment*)phylo_tree->aln)->computeFunctionCat(optimizing_cat, value);
}
double NGSRate::computeFuncDerv(double value, double &df, double &ddf) {
	return ((NGSAlignment*)phylo_tree->aln)->computeFuncDervCat(optimizing_cat, value, df, ddf);
}

void NGSRate::writeInfo(ostream &out) {
}

/****************************************************************************
        NGSTree
 ****************************************************************************/

NGSTree::NGSTree(NGSAlignment *alignment) {
	aln = alignment;
    model = NULL;
    site_rate = NULL;
    optimize_by_newton = true;
    model_factory = NULL;
}

double NGSTree::computeLikelihood(double *pattern_lh) {
	return -((NGSAlignment*)aln)->computeFunction(1.0);
}

double NGSTree::optimizeAllBranches(int iterations) {
	return computeLikelihood();
}

/****************************************************************************
        main function
 ****************************************************************************/

void runNGSAnalysis(Params &params) {
	char model_name[20];
	NGSAlignment aln(params.ngs_file);
	if (params.model_name == "") 
		sprintf(model_name, "GTR+F%d", aln.ncategory);
	else
		sprintf(model_name, "%s+F%d", params.model_name.c_str(), aln.ncategory);
	params.model_name = model_name;
	NGSTree tree(&aln);
	aln.tree = &tree;
    tree.optimize_by_newton = params.optimize_by_newton;
    //tree.sse = params.SSE;
    tree.sse = false;
    tree.setModelFactory(new ModelFactory(params, &tree));
    tree.setModel(tree.getModelFactory()->model);
    tree.setRate(tree.getModelFactory()->site_rate);

    int model_df = tree.getModel()->getNDim() + tree.getRate()->getNDim();
    cout << endl;
    cout << "Model of evolution: " << tree.getModelName() << " (" << model_df << " free parameters)" << endl;

    cout << endl;
    cout << "Optimizing model parameters" << endl;
    cout.precision(10);
    double bestTreeScore = tree.getModelFactory()->optimizeParameters(false);
    cout << "Log-likelihood: " << bestTreeScore << endl;

	string out_file(params.out_prefix);
	out_file += ".ngs";
	ofstream out(out_file.c_str());

	int i, j, k;
	
	double rate_param[aln.num_states * aln.num_states / 2];
	double rate_matrix[aln.num_states * aln.num_states];

	out << "Rate parameters: " << endl;

	tree.getModel()->getRateMatrix(rate_param);

	for (i = 0, k=0; i < aln.num_states-1; i++)
		for (j = i+1; j < aln.num_states; j++, k++)
			rate_matrix[i*aln.num_states+j] = rate_matrix[j*aln.num_states+i] = rate_param[k];

	for (i = 0; i < aln.num_states; i++) {
		for (j = 0; j < aln.num_states; j++) {
			if (j > 0) out << " \t";
			if (j != i) out << rate_matrix[i*aln.num_states+j]; else out << "-";
		}
		out << endl;
	}
	out << endl;
	out << "State frequencies: " << endl;

	double state_freq[aln.num_states];
	tree.getModel()->getStateFrequency(state_freq);

	for (i = 0; i < aln.num_states; i++) out << state_freq[i] << " \t";
	out << endl << endl;

	out << "Position-specific rates: " << endl;
	for (i = 0; i < aln.ncategory; i++)
		out << i+1 << '\t' << tree.getRate()->getRate(i) << endl;
	out.close();
	cout << endl << "Results written to: " << out_file << endl << endl;
}
