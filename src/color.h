#ifndef COLOR_H
#define COLOR_H

namespace soccer {

	class Color {

	public:		
		double R;
		double G;
		double B;
		double A;

		Color( double R, double G, double B, double A ) {
			this->R = R;
			this->G = G;
			this->B = B;
			this->A = A;
		}
	};
};

#endif
