package server

import (
	"log"
	"net"
	"net/http"
	"net/http/httputil"
	"net/url"
	"sync"
	"sync/atomic"
	"time"
)

var pool Pool

// Server represents one of the configurable servers which trafic can be forwarded to.
type Server struct {
	sync.RWMutex
	RP    *httputil.ReverseProxy
	Alive bool
	URL   *url.URL
}

type Pool struct {
	servers []*Server
	curr    uint64
}

// NextPOs increments the curr index in the pool
func (p *Pool) NextPos() uint64 {
	return atomic.AddUint64(&p.curr, uint64(1)) % uint64(len(p.servers))
}

func (s *Server) SetAlive(v bool) {
	s.Lock()
	s.Alive = v
	s.Unlock()
}

func (s *Server) IsAlive() bool {
	s.RLock()
	value := s.Alive
	s.RUnlock()
	return value
}

func (p *Pool) NextServer() *Server {
	index := p.NextPos()
	for i := index; i < (uint64(len(p.servers)) + index); i++ {
		idx := i % uint64(len(p.servers))

		if p.servers[idx].Alive {
			if i != index {
				atomic.StoreUint64(&p.curr, idx)
			}

			return p.servers[idx]
		}
	}

	return nil
}

func Balance(w http.ResponseWriter, r *http.Request) {
	serv := pool.NextServer()
	if serv != nil {
		serv.RP.ServeHTTP(w, r)
		return
	}

	http.Error(w, "cannot access service", http.StatusServiceUnavailable)
}

func AliveCheckURL(u *url.URL) bool {
	conn, err := net.DialTimeout("tcp", u.Host, 2*time.Second)
	if err != nil {
		log.Println("cannot reach server: ", err)
		return false
	}

	conn.Close()
	return true
}

func backgroundHealthCheck() {
	t := time.NewTicker(time.Minute * 3)
	for {
		select {
		case <-t.C:
			log.Printf("checking server health...\n")

			for _, b := range pool.servers {
				status := "up"
				alive := AliveCheckURL(b.URL)
				b.SetAlive(alive)
				if !alive {
					status = "down"
				}

				log.Printf("%s [%s]\n", b.URL, status)
			}

			log.Printf("health check if done...\n")
		}
	}
}
