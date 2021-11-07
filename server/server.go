package server

import (
	"net/http/httputil"
	"net/url"
	"sync"
	"sync/atomic"
)

// Server represents one of the configurable servers which trafic can be forwarded to.
type Server struct {
	sync.RWMutex
	RP      *httputil.ReverseProxy
	IsAlive bool
	URL     *url.URL
}

type Pool struct {
	servers []*Server
	curr    uint64
}

// NextPOs increments the curr index in the pool
func (p *Pool) NextPos() uint64 {
	return atomic.AddUint64(&p.curr, uint64(1)) % uint64(len(p.servers))
}

func (p *Pool) NextServer() *Server {
	index := p.NextPos()
	for i := index; i < (uint64(len(p.servers)) + index); i++ {
		idx := i % uint64(len(p.servers))

		if p.servers[idx].IsAlive {
			if i != index {
				atomic.StoreUint64(&p.curr, idx)
			}

			return p.servers[idx]
		}
	}

	return nil
}
