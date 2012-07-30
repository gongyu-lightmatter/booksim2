// $Id: flit.cpp 4297 2012-02-20 20:39:55Z qtedq $

/*
 Copyright (c) 2007-2011, Trustees of The Leland Stanford Junior University
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 Redistributions of source code must retain the above copyright notice, this 
 list of conditions and the following disclaimer.
 Redistributions in binary form must reproduce the above copyright notice, this
 list of conditions and the following disclaimer in the documentation and/or
 other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*flit.cpp
 *
 *flit struct is a flit, carries all the control signals that a flit needs
 *Add additional signals as necessary. Flits has no concept of length
 *it is a singluar object.
 *
 *When adding objects make sure to set a default value in this constructor
 */

#include "booksim.hpp"
#include "globals.hpp"
#include "flit.hpp"


stack<Flit *> Flit::_all;
stack<Flit *> Flit::_free;

ostream& operator<<( ostream& os, const Flit& f )
{
  os << "  Flit ID: " << f.id << " (" << &f << ")" 
     << " Packet ID: " << f.pid
    //     << " Transaction ID: " << f.tid
     << " Type: " << f.type 
     << " Head: " << f.head
     << " Tail: " << f.tail << endl;
  os << "  Source: " << f.src << "  Dest: " << f.dest << " Intm: "<<f.intm<<endl;
  os << "  Injection time: " << f.time << " Transaction start: "  << "Arrival time: " << f.atime << " Phase: "<<f.ph<< endl;
  os << "  VC: " << f.vc << endl;
  return os;
}


Flit::Flit() 
{  
  Reset();
}  

void Flit::Reset() 
{  
  walkin=true;
  fecn = false;
  becn = false;
  flbid = -1;
  inuse = false;
  res_type = 0;
  type      = ANY_TYPE ;
  vc        = -1 ;
  cl        = -1 ;
  head      = false ;
  tail      = false ;
  ntime = -1;
  time      = -1 ;
  //  ttime     = -1 ;
  atime     = -1 ;
  exptime =-1;
  sn        = -1 ;
  head_sn = -1;
  //  rob_time  = 0 ;
  id        = -1 ;
  pid       = -1 ;
  //  tid       = -1 ;
  flid = -1;
  hops      = 0 ;
  watch     = false ;
  record    = false ;
  intm = 0;
  src = -1;
  dest = -1;
  pri = 0;
  intm =-1;
  ph = -1;
  //dr = -1;
  minimal = 1;
  //ring_par = -1;
  //data = 0;
  payload = -1;
  packet_size=0;
  dest_network_cluster = -1;
  source_network_cluster = -1;
  original_destination = -1;
  going_up_clusters = false;
  ring_par = -1;
  cluster_hops = -1;
  cluster_hops_taken = 0;
  epoch = -1;
  try_again_after_time = -1;
  reservation_size = -1;
  bottleneck_channel_choices.clear();
  reservation_vector.clear();
}  

Flit * Flit::CreateCopy(Flit* f)
{
  Flit* r = New();

  r->dest_network_cluster = f->dest_network_cluster;
  r->source_network_cluster = f->source_network_cluster;
  r->going_up_clusters = f->going_up_clusters;
  r->ring_par = f->ring_par;
  r->cluster_hops = f->cluster_hops;
  r->cluster_hops_taken = f->cluster_hops_taken;
  r->epoch = f->epoch;
  r->try_again_after_time = f->try_again_after_time;
  r->reservation_size = f->reservation_size;
  r->bottleneck_channel_choices.assign(f->bottleneck_channel_choices.begin(), f->bottleneck_channel_choices.end());
  r->reservation_vector.assign(f->reservation_vector.begin(), f->reservation_vector.end());
  r->payload = f->payload;
  r->packet_size = f->packet_size;
  r->reservation_size = f->reservation_size;
  r->ring_par = f->ring_par;
  
  r->walkin = f->walkin;
  r->fecn = f->fecn;
  r->becn = f->becn;
  r->head = f->head;
  r->tail = f->tail;
  r->ntime = f->ntime;
  r->atime = f->atime;
  r->src = f->src;
  r->dest = f->dest;
  r->pri = f->pri;
  r->hops = f->hops;
  r->watch = f->watch;
  r->subnetwork = f->subnetwork;
  r->intm = f->intm;
  r->ph = f->intm;
  r->minimal = f->minimal;
  
  r->exptime = f->exptime;
  
  r->flid  = f->flid;
  r->flbid = f->flbid;
  r->id    = f->id;
  r->pid   = f->pid;

  r->watch = f->watch;
  r->subnetwork = f->subnetwork;
  r->src    = f->src;
  r->time   = f->time;

  r->record = f->record;
  r->cl     = f->cl;
  r->sn     = f->sn;

  
  r->type   = f->type;
  r->pri    = f->pri;
  r->head_sn  = f->head_sn;
  r->res_type = f->res_type;
  r->walkin = f->walkin;

  r->vc = f->vc;
  return r;
}

// This is customized, and thus different than createcopy.
Flit * Flit::Replicate(Flit* f){
  Flit* r = New();

  r->flid  = f->flid;  
  r->id    = f->id+1;
  r->pid   = f->pid;
  assert(f->packet_size>0);
  r->packet_size = f->packet_size-1;
  f->packet_size = 0;

  r->watch = f->watch;
  r->subnetwork = f->subnetwork;
  r->src    = f->src;
  r->time   = f->time;

  r->record = f->record;
  r->cl     = f->cl;
  r->sn     = f->sn+1;

  
  r->type   = f->type;
  r->pri    = f->pri;
  r->head_sn  = f->head_sn;
  r->res_type = f->res_type;
  r->walkin = f->walkin;

  r->vc = f->vc;
  return r;
}

Flit * Flit::New() {
  Flit * f;
  if(_free.empty()) {
    f = new Flit;
    _all.push(f);
    if(_all.size()>10000000){
      cerr<<"Simulation time "<<GetSimTime()<<" flit allocation exceeds "<<_all.size()<<endl;
      exit(-1);
    }
  } else {
    f = _free.top();
    assert(!f->inuse);
    f->Reset();
    _free.pop();
  }
  f->inuse = true;
  return f;
}

void Flit::Free() {
  this->inuse = false;
  _free.push(this);
}

void Flit::FreeAll() {
  while(!_all.empty()) {
    delete _all.top();
    _all.pop();
  }
}


int Flit::OutStanding(){
  return _all.size()-_free.size();
}