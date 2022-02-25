#include "icosaedre.hpp"

using namespace std;

void normalisation(GLfloat* v)
{
  float d=sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
   v[0]=v[0]/d;
   v[1]=v[1]/d;
   v[2]=v[2]/d;
  
}
void produit_vectoriel(float* a, float* b, float* c, float *vectnormal)
{
  vectnormal[0] = (b[1]-a[1])*(c[2]-a[2])-(b[2]-a[2])*(c[1]-a[1]);
  vectnormal[1] = (b[2]-a[2])*(c[0]-a[0])-(b[0]-a[0])*(c[2]-a[2]);
  vectnormal[2] = (b[0]-a[0])*(c[1]-a[1])-(b[1]-a[1])*(c[0]-a[0]);

}
/*
 * nbtriangles est le nombre initial de triangles
 * idem pour nbsommets
 * sommets est le tableau des coordonnées des sommets de l'icosaèdre à raffiner
 * newsommets est le tableau construit dans la fonction et correspond à l'ensemble des sommets de l'icosaèdre raffiné
 * index est le tableau d'indices de l'icosaèdre à raffiner
 * newindex est le tableau construit dans la fonction et correspond à l'ensemble des indices associés à newsommets de l'icosaèdre raffiné
 * ATTENTION il faut alloué newsommets et newindex avant l'appel de la fonction
 * newsommets est de taille 3*(nbsommets + 3*nbtriangles/2)
 * newindex est de taille 3*(4*nbtriangles)
 */
void subdivision_optimise(int nbtriangles, int nbsommets, GLfloat* sommets, GLfloat* newsommets, GLuint* index, GLuint* newindex)
{

    for (int i=0; i<3*nbsommets; i++)
        newsommets[i] = sommets[i];

    int* tester = new int[nbsommets*nbsommets];

    for (int i=0; i<nbsommets*nbsommets; i++)
        tester[i] = -1;

    int ptrindex = 0;
    GLuint newnbsommets = nbsommets;
    GLfloat v0[3];
    GLfloat v1[3];
    GLfloat v2[3];
    GLfloat v3[3];
    GLfloat v4[3];
    GLfloat v5[3];
    GLuint ind0, ind1, ind2;
    for (int i=0; i<nbtriangles; i++) {
        ind0 = index[3 * i];
        ind1 = index[3 * i + 1];
        ind2 = index[3 * i + 2];
        for (int j = 0; j < 3; j++) {
            v0[j] = sommets[3 * ind0 + j];
            v1[j] = sommets[3 * ind1 + j];
            v2[j] = sommets[3 * ind2 + j];
        }
        GLuint indplus0, indplus1, indplus2;
        if (tester[ind0 * nbsommets + ind1] != -1)
            indplus0 = tester[ind0 * nbsommets + ind1];
        else {
            for (int j = 0; j < 3; j++)
                v3[j] = (v0[j] + v1[j]) / 2;
            normalisation(v3);
            tester[ind0 * nbsommets + ind1] = newnbsommets;
            tester[ind1 * nbsommets + ind0] = newnbsommets;
            indplus0 = newnbsommets;

            for (int j = 0; j < 3; j++)
                newsommets[3 * newnbsommets + j] = v3[j];
            newnbsommets++;
        }
        if (tester[ind1 * nbsommets + ind2] != -1) {
            indplus1 = tester[ind1 * nbsommets + ind2];
        } else {
            for (int j = 0; j < 3; j++)
                v4[j] = (v1[j] + v2[j]) / 2;
            normalisation(v4);
            tester[ind1 * nbsommets + ind2] = newnbsommets;
            tester[ind2 * nbsommets + ind1] = newnbsommets;
            indplus1 = newnbsommets;

            for (int j = 0; j < 3; j++)
                newsommets[3 * newnbsommets + j] = v4[j];
            newnbsommets++;
        }
        if (tester[ind2 * nbsommets + ind0] != -1) {
            indplus2 = tester[ind2 * nbsommets + ind0];
        } else {
            for (int j = 0; j < 3; j++)
                v5[j] = (v2[j] + v0[j]) / 2;
            normalisation(v5);
            tester[ind2 * nbsommets + ind0] = newnbsommets;
            tester[ind0 * nbsommets + ind2] = newnbsommets;
            indplus2 = newnbsommets;
            for (int j = 0; j < 3; j++)
                newsommets[3 * newnbsommets + j] = v5[j];
            newnbsommets++;
        }
        newindex[ptrindex] = ind0; //v0
        ptrindex++;
        newindex[ptrindex] = indplus0; //v3
        ptrindex++;
        newindex[ptrindex] = indplus2; // v5
        ptrindex++;
        newindex[ptrindex] = indplus0; //v3
        ptrindex++;
        newindex[ptrindex] = ind1; //v1
        ptrindex++;
        newindex[ptrindex] = indplus1; //v4
        ptrindex++;
        newindex[ptrindex] = indplus0; //v3
        ptrindex++;
        newindex[ptrindex] = indplus1; //v4
        ptrindex++;
        newindex[ptrindex] = indplus2; //v5
        ptrindex++;
        newindex[ptrindex] = indplus2;//v5
        ptrindex++;
        newindex[ptrindex] = indplus1; //v4
        ptrindex++;
        newindex[ptrindex] = ind2; //v2
        ptrindex++;
    }
    delete[] tester;
}


