//---------------------------------------------------------------------------
/*
�\�P�b�g�̃��b�p Ver.1.3.2  by Gimite �s��

TCP/IP�\�P�b�g�̃��b�p�ł��BWindows�iWinSock2�j��UNIX�iPOSIX Socket�j�ɑ�
�����Ă��܂��B

Gimite �s�� <gimite@mx12.freecom.ne.jp>

*/
//---------------------------------------------------------------------------

#include <Foundation/UTSocket.h>
#ifdef GIMITE_WIN32
#  include <WinSock2.h>

#  pragma comment(lib,"ws2_32.lib")
#else
#  include <sys/socket.h>
#  include <netinet/in.h>
#  include <arpa/inet.h>
#  include <netdb.h>
#  include <unistd.h>
#endif
//---------------------------------------------------------------------------
namespace gimite{
//�\�P�b�g�V�X�e����������
	
	bool startup_socket(){
#ifdef GIMITE_WIN32
		WSADATA wsaData;
		return WSAStartup(0x0002, &wsaData)==0;
#else
		return true;
#endif
}

//---------------------------------------------------------------------------
	//�\�P�b�g�V�X�e������n��
	void cleanup_socket(){
#ifdef GIMITE_WIN32
		WSACleanup();
#endif
}

	//IP�A�h���X

	ip_address::ip_address(sock_uint32_t  addr = INADDR_NONE) {
		addr_ = new in_addr;
#ifdef GIMITE_WIN32
		addr_->S_un.S_addr= addr;
#else
		addr_->s_addr= addr;
#endif
	}
	
	//ip_address::ip_address(const in_addr& addr) : addr_(addr){}
	ip_address::ip_address(const in_addr& addr) {
		addr_ = new in_addr(addr);
	}
	ip_address::ip_address(const std::string& host) {
		solve_address(host.c_str());
	}

	ip_address::ip_address(const char* host){
		solve_address(host);
	}
	
	ip_address::~ip_address() {
		delete(addr_);
		addr_ = 0;
	}

	ip_address& ip_address::operator=(const std::string& host){
		solve_address(host.c_str());
		return *this;
	}

	ip_address& ip_address::operator=(const char* host){
		solve_address(host);
		return *this;
	}

	in_addr* ip_address::addr()const{
		return addr_;
	}
	sock_uint32_t ip_address::as_int()const {
#ifdef GIMITE_WIN32
		return addr_->S_un.S_addr;
#else
		return addr_->s_addr;
#endif
	}

	void ip_address::solve_address(const char* host){
		sock_uint32_t uaddr= ::inet_addr(host);
		if (uaddr==INADDR_NONE){
			hostent* ent= ::gethostbyname(host);
			if (ent && ent->h_addr_list[0])
				uaddr= *(sock_uint32_t*)ent->h_addr_list[0];
		}
#ifdef GIMITE_WIN32
		addr_->S_un.S_addr= uaddr;
#else
		addr_->s_addr= uaddr;
#endif
	}
	
	bool operator<(const ip_address& lhs, const ip_address& rhs){
		return lhs.as_int()<rhs.as_int();
	}
	bool operator>(const ip_address& lhs, const ip_address& rhs){ return rhs<lhs; }
	bool operator<=(const ip_address& lhs, const ip_address& rhs){ return !(lhs>rhs); }
	bool operator>=(const ip_address& lhs, const ip_address& rhs){ return !(lhs<rhs); }
	bool operator==(const ip_address& lhs, const ip_address& rhs){ return lhs>=rhs && lhs<=rhs; }
	bool operator!=(const ip_address& lhs, const ip_address& rhs){ return !(lhs==rhs); }
	
	std::ostream& operator<<(std::ostream& os, const ip_address& addr){
		sock_uint32_t uaddr= addr.as_int();
		for (int i= 0; i<4; ++i){
			if (i>0) os << ".";
			os << (int)(unsigned char)(uaddr >> i*8);
		}
		return os;
	}
	
	
	//---------------------------------------------------------------------------
	//IP�A�h���X+�|�[�g�ԍ�

	socket_address::socket_address(){
		ip = new ip_address();
	}
	socket_address::socket_address(ip_address& i= ip_address(), int p= 0)
		: ip(&i), port(p){}
	socket_address::~socket_address(){
		delete(ip);
		ip = 0;
	}
	
	//��r�A���l����B
	bool operator<(const socket_address& lhs, const socket_address& rhs){
		if (lhs.ip<rhs.ip) return true;
		if (lhs.ip>rhs.ip) return false;
		return lhs.port<rhs.port;
	}
	bool operator>(const socket_address& lhs, const socket_address& rhs){ return rhs<lhs; }
	bool operator<=(const socket_address& lhs, const socket_address& rhs){ return !(lhs>rhs); }
	bool operator>=(const socket_address& lhs, const socket_address& rhs){ return !(lhs<rhs); }
	bool operator==(const socket_address& lhs, const socket_address& rhs){
		return lhs>=rhs && lhs<=rhs;
	}
	bool operator!=(const socket_address& lhs, const socket_address& rhs){ return !(lhs==rhs); }

	//stream�o�́B
	std::ostream& operator<<(std::ostream& os, const socket_address& saddr){
		os << saddr.ip << ":" << saddr.port;
		return os;
	}
	
	//---------------------------------------------------------------------------
	//�X�g���[���\�P�b�g�pstreambuf
	socket_streambuf::socket_streambuf(socket_t sock= -1, std::size_t buf_size= 1024)
		: sock_(sock), buffer_(buf_size){}
	socket_streambuf::socket_streambuf(const ip_address& host, int port, 
		std::size_t buf_size= 1024)
		: sock_(-1), buffer_(buf_size){
			open(host, port);
	}
	socket_streambuf::~socket_streambuf(){
		close();
	}
	//�ڑ�
	bool socket_streambuf::open(const ip_address& host, int port){
		if (is_open()) close();
		if (host.as_int()==INADDR_NONE) return false;
		sock_= ::socket(AF_INET, SOCK_STREAM, 0);
		if (sock_==-1) return false;
		sockaddr_in addr;
		addr.sin_family= AF_INET;
		addr.sin_port= htons((unsigned short)port);
		addr.sin_addr= *host.addr();
		if (::connect(sock_, (const sockaddr*)&addr, sizeof(sockaddr_in))==-1)
			close();
		return is_open();
	}
	//�ؒf
	void socket_streambuf::close(){
		if (sock_==-1) return;
#ifdef GIMITE_WIN32
		::closesocket(sock_);
#else
		::close(sock_);
#endif
		sock_= -1;
	}
	//�\�P�b�g�n���h��������ɉ��
	socket_t socket_streambuf::release(){
		socket_t sock= sock_;
		sock_= -1;
		return sock;
	}
	//���M
	int socket_streambuf::send(const void* buffer, int size){
		return ::send(sock_, (const char*)buffer, size, 0);
	}
	//��M
	int socket_streambuf::recv(void* buffer, int size){
		return ::recv(sock_, (char*)buffer, size, 0);
	}
	//����ɐڑ�����Ă邩
	bool socket_streambuf::is_open()const{ return sock_!=-1; }
	//�\�P�b�g�n���h��
	socket_t socket_streambuf::socket()const{ return sock_; }

	//1�������M
	socket_streambuf::int_type socket_streambuf::overflow(int_type c= std::char_traits<char>::eof()){
		if (!traits_type::eq_int_type(c, traits_type::eof())){
			char cc= char(c);
			if (traits_type::eq_int_type(xsputn(&cc, 1), 0))
				return traits_type::eof();
		}
		return 'a';
	}
	//n�������M
	std::streamsize socket_streambuf::xsputn(const char_type* s, std::streamsize n){
		if (!is_open()) return 0;
		sock_result_t res= ::send(sock_, s, int(n), 0);
		return res==-1? 0 : res;
	}
	//��M�o�b�t�@����ɂȂ���
	socket_streambuf::int_type socket_streambuf::underflow(){
		if (!is_open()) return traits_type::eof();
		if (!gptr() || gptr()>=egptr()){
			if (gptr() && gptr()>eback())
				buffer_[0]= *(gptr()-1);
			else
				buffer_[0]= '\0';
			sock_result_t size= ::recv(sock_, &buffer_[1], int(buffer_.size()-1), 0);
			if (size<=0)
				return traits_type::eof();
			else
				setg(&buffer_[0], &buffer_[1], &buffer_[size+1]);
		}
		return traits_type::to_int_type(*gptr());
	}
	
	//---------------------------------------------------------------------------
	//�X�g���[���\�P�b�g�pstream
	//�����ɂ͐ڑ����Ȃ��R���X�g���N�^
	socket_stream::socket_stream(): std::iostream(0){
		rdbuf(&sbuf_);
		setstate(std::ios::badbit);
	}
	//�����ɐڑ�����R���X�g���N�^
	socket_stream::socket_stream(const ip_address& host, int port)
		: std::iostream(0), sbuf_(host, port){
			rdbuf(&sbuf_);
			if (!sbuf_.is_open()) setstate(std::ios::badbit);
	}
	//�����̃\�P�b�g���w�肷��R���X�g���N�^
	socket_stream::socket_stream(socket_t sock)
		: std::iostream(0), sbuf_(sock){
			rdbuf(&sbuf_);
			if (!sbuf_.is_open()) setstate(std::ios::badbit);
	}
	//�f�X�g���N�^
	socket_stream::~socket_stream(){ rdbuf(0); }
	//�ڑ�
	void socket_stream::open(const ip_address& host, int port){
		clear();
		sbuf_.open(host, port);
		if (!sbuf_.is_open()) setstate(std::ios::badbit);
	}
	//�ؒf
	void socket_stream::close(){
		setstate(std::ios::badbit);
		sbuf_.close();
	}
	//�\�P�b�g�n���h��������ɉ��
	socket_t socket_stream::release(){
		setstate(std::ios::badbit);
		return sbuf_.release();
	}
	//���M
	int socket_stream::send(const void* buffer, int size){ return sbuf_.send(buffer, size); }
	//��M
	int socket_stream::recv(void* buffer, int size){ return sbuf_.recv(buffer, size); }
	//�\�P�b�g�n���h��
	socket_t socket_stream::socket()const{ return sbuf_.socket(); }
	
	//---------------------------------------------------------------------------
	//�o�C���h���Ďg���\�P�b�g
	//�ʏ�̃R���X�g���N�^
	bound_socket::bound_socket(int type)
		: type_(type), sock_(-1), is_bound_(false){}
	//�����̃\�P�b�g�n���h�����g���R���X�g���N�^
	bound_socket::bound_socket(int type, socket_t sock, bool is_bound)
		: type_(type), sock_(sock), is_bound_(is_bound){}
	//�f�X�g���N�^
	bound_socket::~bound_socket(){
		close();
	}
	//�\�P�b�g���N���[�Y
	void bound_socket::close(){
		is_bound_= false;
		if (sock_==-1) return;
#ifdef GIMITE_WIN32
		::closesocket(sock_);
#else
		::close(sock_);
#endif
		sock_= -1;
	}
	//�\�P�b�g�n���h�������
	socket_t bound_socket::release(){
		socket_t sock= sock_;
		sock_= -1;
		is_bound_= false;
		return sock;
	}
	//�����̃\�P�b�g���w��
	void bound_socket::socket(socket_t sock, bool is_bound){
		close();
		sock_= sock;
		is_bound_= is_bound;
	}
	//�\�P�b�g�̃^�C�v
	int bound_socket::type()const{ return type_; }
	//�\�P�b�g�n���h��
	socket_t bound_socket::socket()const{ return sock_; }
	//�������o�C���h����Ă��邩
	bound_socket::operator const void*()const{ return is_bound_? this : NULL; }
	bool bound_socket::operator!()const{ return !is_bound_; }


	//�o�C���h
	bool bound_socket::pbind(int port){
		if (is_bound_) return false;
		if (sock_==-1){
			sock_= ::socket(AF_INET, type_, 0);
			if (sock_==-1) return false;
		}
		sockaddr_in addr;
		addr.sin_family= AF_INET;
		addr.sin_port= htons(port);
		addr.sin_addr= *ip_address(sock_uint32_t(INADDR_ANY)).addr();
		//���Fcygwin gcc 3.3.1�ł�INADDR_ANY�̌^��sock_uint32_t�ł͂Ȃ��̂ŁA
		//    �B�����𖳂������߂̃L���X�g���K�v�B
		if (::bind(sock_, (const sockaddr*)&addr, sizeof(addr))==-1)
			return false;
		is_bound_= true;
		return true;
	}

	
	//---------------------------------------------------------------------------
	//�T�[�o�p�X�g���[���\�P�b�g
	//�����ɂ̓o�C���h���Ȃ��R���X�g���N�^
	server_stream_socket::server_stream_socket()
		: bound_socket(SOCK_STREAM){}
	//�����Ƀo�C���h����R���X�g���N�^
	server_stream_socket::server_stream_socket(int port, int backlog= 5)
		: bound_socket(SOCK_STREAM){
			bind(port, backlog);  
	}
	//�����̃\�P�b�g�n���h�����g���R���X�g���N�^
	server_stream_socket::server_stream_socket(socket_t sock, bool is_bound)
		: bound_socket(SOCK_STREAM, sock, is_bound){}
	//�o�C���h
	bool server_stream_socket::bind(int port, int backlog= 5){
		if (!pbind(port)) return false;
		if (::listen(socket(), backlog)!=-1) return true;
		close();
		return false;
	}
	//�ڑ��v�����󂯓����
	socket_t server_stream_socket::accept(){
		return ::accept(socket(), 0, 0);
	}

	
	//---------------------------------------------------------------------------
	//�_�C�A�O�����\�P�b�g
	//�����ɂ̓o�C���h���Ȃ��R���X�g���N�^
	diagram_socket::diagram_socket()
		: bound_socket(SOCK_DGRAM){}
	//�����Ƀo�C���h����R���X�g���N�^
	diagram_socket::diagram_socket(int port)
		: bound_socket(SOCK_DGRAM){
			bind(port);
	}
	//�����̃\�P�b�g�n���h�����g���R���X�g���N�^
	diagram_socket::diagram_socket(socket_t sock, bool is_bound)
		: bound_socket(SOCK_DGRAM, sock, is_bound){}
	//�o�C���h
	bool diagram_socket::bind(int port){
		return pbind(port);
	}
	//�f�[�^����M
	int diagram_socket::recvfrom(void* buffer, int size, socket_address* addr,
		int flags){
			struct sockaddr_in saddr;
			sock_len_t addr_len= sizeof(saddr);
			int result= ::recvfrom(socket(), (char*)buffer, size, flags,
				(struct sockaddr *)&saddr, &addr_len);
			//���FWinsock��recvfrom�̑�2�p�����[�^�^��char*�Ȃ̂�
			//    �L���X�g���K�v�B
			if (addr){
				(addr->ip)= &ip_address(saddr.sin_addr);
				addr->port= ntohs(saddr.sin_port);
			}
			return result;
	}
	//�f�[�^�𑗐M
	int diagram_socket::sendto(const void* buffer, int size, const socket_address* addr,
		int flags= 0){
			sockaddr_in saddr;
			saddr.sin_family= AF_INET;
			saddr.sin_addr= *addr->ip->addr();
			saddr.sin_port= htons(addr->port);
			return ::sendto(socket(), (const char*)buffer, size, flags,
				(const sockaddr*)&saddr, sizeof(saddr));
			//���FWinsock��sendto�̑�2�p�����[�^�^��const char*�Ȃ̂�
			//    �L���X�g���K�v�B
	}
	
}	//namespace