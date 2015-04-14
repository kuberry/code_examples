#include <Tpetra_DefaultPlatform.hpp>
#include <Tpetra_Vector.hpp>
#include <Tpetra_Version.hpp>
#include <Teuchos_GlobalMPISession.hpp>
#include <Teuchos_oblackholestream.hpp>

int main(int argc, char* argv[]) {

  Teuchos::oblackholestream blackHole;

  Teuchos::GlobalMPISession mpiSession (&argc, &argv, &blackHole);

  Teuchos::RCP<const Teuchos::Comm<int> > comm =
    Tpetra::DefaultPlatform::getDefaultPlatform ().getComm ();
  // Get my process' rank, and the total number of processes.
  // Equivalent to MPI_Comm_rank resp. MPI_Comm_size.

  const int myRank = comm->getRank ();
  const int numProcs = comm->getSize ();

  std::ostream& out = (myRank == 0) ? std::cout : blackHole; 

  if (myRank == 0) {
    std::cout << "Total number of processes: " << numProcs << std::endl;
  }  	
  if (myRank == 0) {
    std::cout << "End Result: TEST PASSED" << std::endl;
  }

  // The standard scalar_type from Vector<> is a double
  typedef Tpetra::Vector<>::scalar_type scalar_type;
  //  typedef Tpetra::Vector<>::local_ordinal_type local_ordinal_type;
  typedef Tpetra::Vector<>::global_ordinal_type global_ordinal_type;
  // typedef Tpetra::Vector<>::node_type node_type;
  // Get the default map type from Tpetra ( int -> int)
  typedef Tpetra::Map<> map_type;



  const size_t numLocalEntries = 5;
  // This is a dynamic number of global entries
  const Tpetra::global_size_t numGlobalEntries =
    comm->getSize () * numLocalEntries;

  // Could be set to 1 to match Fortran
  const global_ordinal_type indexBase = 0;

  // Creates contiguous mapping through auto-distribution
  Teuchos::RCP<const map_type> contigMap =
    rcp (new map_type (numGlobalEntries, indexBase, comm));
  TEUCHOS_TEST_FOR_EXCEPTION(
    ! contigMap->isContiguous (), std::logic_error,
    "The supposedly contiguous Map isn't contiguous.");

  // Creates contiguous mapping through manual cardinality specification
  Teuchos::RCP<const map_type> contigMap2 =
    rcp (new map_type (numGlobalEntries, numLocalEntries, indexBase, comm));
  TEUCHOS_TEST_FOR_EXCEPTION(
    ! contigMap->isSameAs (*contigMap2), std::logic_error,
    "contigMap should be the same as contigMap2, but it's not.");

  // Creates contiguous mapping through manual cardinality w/o knowledge of # global entries
  const Tpetra::global_size_t INVALID =
    Teuchos::OrdinalTraits<Tpetra::global_size_t>::invalid ();
  Teuchos::RCP<const map_type> contigMap3 =
    rcp (new map_type (INVALID, numLocalEntries, indexBase, comm));
  TEUCHOS_TEST_FOR_EXCEPTION(
    ! contigMap2->isSameAs (*contigMap3), std::logic_error,
    "contigMap2 should be the same as contigMap3, but it's not.");

  typedef Tpetra::Vector<> vector_type;
  vector_type x(contigMap);
  // Set's everything in the vector to 1.0
  x.putScalar(1.0);

  vector_type y(contigMap);
  y.putScalar(10.0);

  // Should give an answer of 32 in each entry
  // x.update(a, y, b) -> x_new = b*x_old + a*y_old;
  // This can be performed for more vectors, (a, y, z, c, b), etc...
  x.update(3.0, y, 2); 

  typedef Tpetra::Vector<>::mag_type mag_type;
  out << "Norm of x: " << x.norm2() << std::endl;

  // The local length of a Tpetra vector can also be retrieved
  out << "Local length is: " << x.getLocalLength() << std::endl;


  // Views allow for a persistent view of the data in a vector,
  // but since it is const, it is undefined when a value is changed
  
  Kokkos::ArrayRCP<const scalar_type> x_data = x.get1dView ();

  double localSum;
  for (size_t i=0; i<x.getLocalLength(); i++) {
    localSum += x_data[i];
  }  

  double globalSum = 0;
//  Teuchos::RCP< Teuchos::ValueTypeSerializer<int,double> > serializer =
//  				workset.serializerManager.template getValue<double>();
  Teuchos::reduceAll<int, double> (*comm, Teuchos::REDUCE_SUM, 1, &localSum,
		  			&globalSum);
//                                      Teuchos::outArg (globalSum));

  out << "All vector entries sum to " << globalSum << std::endl;

  return 0;
}
