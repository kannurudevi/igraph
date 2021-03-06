\name{multilevel.community}
\alias{multilevel.community}
\concept{Community structure}
\concept{Multilevel community detection}
\title{Finding community structure by multi-level optimization of
  modularity}
\description{This function implements the multi-level modularity optimization
  algorithm for finding community structure, see references below. It
  is based on the modularity measure and a hierarchial approach.
}
\usage{
multilevel.community (graph, weights = NULL) 
}
\arguments{
  \item{graph}{The input graph.}
  \item{weights}{Optional positive weight vector.
    If the graph has a \code{weight} edge attribute, then this is used
    by default. Supply \code{NA} here if the graph has a \code{weight}
    edge attribute, but you want to ignore it.}
}
\details{
  This function implements the multi-level modularity optimization
  algorithm for finding community structure, see 
  VD Blondel, J-L Guillaume, R Lambiotte and E Lefebvre: Fast unfolding of
  community hierarchies in large networks,
  \url{http://arxiv.org/abs/arXiv:0803.0476}
  for the details.

  It is based on the modularity measure and a hierarchial approach. 
  Initially, each vertex is assigned to a community on its own. In every
  step, vertices are re-assigned to communities in a local, greedy way:
  each vertex is moved to the community with which it achieves the
  highest contribution to modularity. When no vertices can be
  reassigned, each community is considered a vertex on its own, and the
  process starts again with the merged communities. The process stops
  when there is only a single vertex left or when the modularity
  cannot be increased any more in a step.
  
  This function was contributed by Tom Gregorovic.
}
\value{
  \code{multilevel.community} returns a \code{\link{communities}}
  object, please see the \code{\link{communities}} manual page for
  details.
}
\references{
  Vincent D. Blondel, Jean-Loup Guillaume, Renaud Lambiotte, Etienne
  Lefebvre: Fast unfolding of communities in large
  networks. J. Stat. Mech. (2008) P10008   
}
\author{Tom Gregorovic, Tamas Nepusz \email{tamas@cs.rhul.ac.uk}}
\seealso{
  See \code{\link{communities}} for extracting the membership,
  modularity scores, etc. from the results.
  
  Other community detection algorithms:
  \code{\link{walktrap.community}},
  \code{\link{spinglass.community}},
  \code{\link{leading.eigenvector.community}},
  \code{\link{edge.betweenness.community}},
  \code{\link{fastgreedy.community}},
  \code{\link{label.propagation.community}}
}
\examples{
# This is so simple that we will have only one level
g <- graph.full(5) \%du\% graph.full(5) \%du\% graph.full(5)
g <- add.edges(g, c(1,6, 1,11, 6, 11))
multilevel.community(g)
}
\keyword{graphs}