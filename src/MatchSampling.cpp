
    # include <iostream>
    # include <vector>
    # include <fstream>
    # include <cmath>

    # define S_PI ( 3.14159265358979323846264338327950288419716939937510L )

    typedef struct s_point_struct {

        float x;
        float y;

    } s_point;

    typedef struct s_vector_struct {

        float x;
        float y;
        float z;

    } s_vector;

    s_vector s_match_cartesian( long const s_width, long const s_height, s_point const  & s_p ) {

        /* returned structure variable */
        s_vector s_return;

        /* coordinates re-normalisation */
        float s_phi = ( s_p.x / s_width ) * 2.0 * S_PI;

        /* coordinates re-normalisation */
        float s_theta = ( ( s_p.y / ( s_height - 1 ) ) - 0.5 ) * S_PI;

        /* compute cartesian coordinates */
        s_return.x = cos( s_theta ) * cos( s_phi );
        s_return.y = cos( s_theta ) * sin( s_phi );
        s_return.z = sin( s_theta );

        /* return converted coordinates */
        return( s_return );

    }

    float s_match_scalar( s_vector & s_candidate, s_vector & s_direction ) {

        /* compute and return angle */
        return( s_candidate.x * s_direction.x + s_candidate.y * s_direction.y + s_candidate.z * s_direction.z );

    }

    int s_match_read( char const * const s_file, std::vector < s_point > & s_m1, std::vector < s_point > & s_m2, std::vector < s_point > & s_m3 ) {

        /* reading variables */
        s_point s_m1r;
        s_point s_m2r;
        s_point s_m3r;

        /* stream variable */
        std::fstream s_stream;

        /* clear arrays */
        s_m1.clear();
        s_m2.clear();
        s_m3.clear();

        /* create stream */
        s_stream.open( s_file, std::ios::in );

        /* check stream */
        if ( s_stream.is_open() == false ) {

            /* send message */
            return( 1 );

        }

        /* import file data */
        while ( s_stream >> s_m1r.x >> s_m1r.y >> s_m2r.x >> s_m2r.y >> s_m3r.x >> s_m3r.y ) {

            /* push element */
            s_m1.push_back( s_m1r );
            s_m2.push_back( s_m2r );
            s_m3.push_back( s_m3r );

        }

        /* delete stream */
        s_stream.close();

        /* send message */
        return( 0 );

    }

    int s_match_write( char const * const s_file, std::vector < s_point > const & s_m1, std::vector < s_point > const & s_m2, std::vector < s_point > const & s_m3 ) {

        /* stream variable */
        std::fstream s_stream;

        /* create stream */
        s_stream.open( s_file, std::ios::out );

        /* check stream */
        if ( s_stream.is_open() == false ) {

            /* send message */
            return( 1 );

        }

        /* stream precision */
        s_stream.precision( 12 );

        /* parsing match array */
        for ( long s_parse( 0 ); s_parse < s_m1.size(); s_parse ++ ) {

            /* export match */
            s_stream << std::fixed
                     << s_m1[s_parse].x << " "
                     << s_m1[s_parse].y << " "
                     << s_m2[s_parse].x << " "
                     << s_m2[s_parse].y << " "
                     << s_m3[s_parse].x << " "
                     << s_m3[s_parse].y << std::endl;

        }

        /* close stream */
        s_stream.close();

        /* send message */
        return( 0 );

    }

    void s_match_filter( float const s_tol, long const s_width, long const s_height, std::vector < s_point > const & s_m1, std::vector < s_point > const & s_m2, std::vector < s_point > const & s_m3, std::vector < s_point > & s_m1out, std::vector < s_point > & s_m2out, std::vector < s_point > & s_m3out ) {

        /* direction array variable */
        std::vector < s_vector > s_array;

        /* direction vector variable */
        s_vector s_direction;

        /* parsing variable */
        long s_index( 0 );

        /* selection flag variable */
        bool s_flag( true );

        /* scalar product variable */
        float s_scalar( 0.0 );

        /* clear arrays */
        s_m1out.clear();
        s_m2out.clear();
        s_m3out.clear();

        /* parsing matches - reference on s_m2 */
        for ( long s_parse( 0 ); s_parse < s_m2.size(); s_parse ++ ) {

            /* compute direction */
            s_direction = s_match_cartesian( s_width, s_height, s_m2[s_parse] );

            /* reset index */
            s_index = s_array.size() - 1;

            /* reset flag */
            s_flag = true;

            /* parsing directions */
            while ( ( s_index >= 0 ) && ( s_flag == true ) ) {

                /* compute and check scalar product */
                if ( ( s_scalar = s_match_scalar( s_direction, s_array[s_index] ) ) > 0.0 ) {

                    /* compute and check angle */
                    if ( acos( s_scalar ) < s_tol ) {

                        /* update flag */
                        s_flag = false;

                    }

                }

                /* update index */
                s_index --;

            }

            /* check flag */
            if ( s_flag == true ) {

                /* push computed direction */
                s_array.push_back( s_direction );

                /* select subsequent match */
                s_m1out.push_back( s_m1[s_parse] );
                s_m2out.push_back( s_m2[s_parse] );
                s_m3out.push_back( s_m3[s_parse] );

            }

        }

    }

    int main( int argc, char ** argv ) {

        /* match coordinates variable */
        std::vector < s_point > s_m1;
        std::vector < s_point > s_m2;
        std::vector < s_point > s_m3;
        std::vector < s_point > s_f1;
        std::vector < s_point > s_f2;
        std::vector < s_point > s_f3;

        /* image size variable */
        long s_width ( 0 );
        long s_height( 0 );

        /* tolerance value variable */
        float s_tol( 0.0 );

        /* check consistency */
        if ( argc != 6 ) {

            /* display message */
            std::cerr << "scanvan : error : wrong usage" << std::endl;

            /* display quick help */
            std::cerr << "./HomogenousMatch [triplet matches input path] [image width in pixels] [image height in pixels] [tolerance value] [triplet match output path]" << std::endl;

            /* send message */
            return( 1 );

        }

        /* read image size */
        s_width  = atol( argv[2] );
        s_height = atol( argv[3] );

        /* read tolerance value */
        s_tol = atof( argv[4] );

        /* read match file */
        if ( s_match_read( argv[1], s_m1, s_m2, s_m3 ) != 0 ) {

            /* display message */
            std::cerr << "scanvan : error : unable to load matches" << std::endl;

            /* send message */
            return( 1 );

        }

        /* match resampling process */
        s_match_filter( s_tol, s_width, s_height, s_m1, s_m2, s_m3, s_f1, s_f2, s_f3 );

        /* export match */
        s_match_write( argv[5], s_f1, s_f2, s_f3 );

        /* send message */
        return( 0 );

    }

