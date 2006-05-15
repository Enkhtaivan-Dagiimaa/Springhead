#ifndef BASE_COMBINATION_H
#define BASE_COMBINATION_H

/**	@file Combination.h �g�ݍ��킹�z��̒�`*/
namespace Spr {;

/**	�g�ݍ��킹�z��i����ւ��s�C4�p�`�ɂȂ�j*/
template <class T>
class UTCombination:public std::vector<T>{
	int height_;
	int width_;
public:
	typedef std::vector<T> base_type;
	UTCombination(){ height_ = 0; width_ = 0; }
	
	///	�T�C�Y�̕ύX
	/// [0, min(height(), hnew))�s�C[0, min(width(), wnew)��̗v�f�͕ۑ������
	/// �V�����ł���v�f�ɂ�T()����������
	void resize(int hnew, int wnew){
		if (hnew * wnew > height_ * width_){	//	�傫���Ȃ�Ȃ�C��Ƀ��T�C�Y
			base_type::resize(hnew * wnew);
		}
		typename base_type::iterator b = this->begin();
		int hmin = min(hnew, height_);
		int r, c;
		if (wnew > width_){	//	����������ꍇ�C��납��ړ�
			for(r = hmin-1; r >= 0; --r){
				for(c = wnew-1; c >= width_; --c)
					b[wnew * r + c] = T();
				for(; c >= 0; --c)
					b[wnew * r + c] = b[width_ * r + c];
			}
		}
		else if (wnew < width_){	//	��������ꍇ�C�O����ړ�
			for(r = 1; r < hmin; ++r){
				for(c = 0; c < wnew; ++c)
					b[wnew * r + c] = b[width_ * r + c];
			}
		}
		if (hnew * wnew < height_ * width_)	// �������Ȃ�Ȃ�Ō�Ƀ��T�C�Y
			base_type::resize(hnew * wnew);

		height_ = hnew;
		width_ = wnew;
	}
	/// r�Ԗڂ̍s���폜���C[r+1, height())�Ԗڂ̍s��1��ɋl�߂�
	void erase_row(int row){
		assert(0 <= row && row < height_);
		if(row != height_ - 1){
			int r, c;
			for(r = row+1; r < height_; r++)
				for(c = 0; c < width_; c++)
					item(r, c) = item(r-1, c);
		}
		height_--;
		base_type::resize(height_ * width_);
	}
	/// c�Ԗڂ̗���폜���C[c+1, width())�Ԗڂ̗��1���ɋl�߂�
	void erase_col(int col){
		assert(0 <= col && col < width_);
		if(col != width_ - 1){
			int r, c;
			for(c = col+1; c < width_; c++)
				for(r = 0; r < height_; r++)
					item(r, c) = item(r, c-1);
		}
		width_--;
		base_type::resize(height_ * width_);
	}
	int height() const { return height_; };
	int width() const { return width_; };
	T& item(int i, int j){
		assert(i < height());
		assert(j < width());
		return this->begin()[i*width() + j];
	}
	void clear(){
		base_type::clear();
		height_ = 0;
		width_ = 0;
	}
	const T& item(int i, int j) const { return ((UTCombination<T>*)this)->item(i,j); }
};

}

#endif
