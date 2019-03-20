<?php

function lab_migration_download_lab_pdf()
{
  $lab_id = arg(2);
  _latex_copy_script_file();
  $full_lab = arg(3);
  if ($full_lab == "1"){
  	_latex_generate_files($lab_id, TRUE);
  }else{
  	_latex_generate_files($lab_id, FALSE);
}
}


/* full lab parameter is for gennerating the entire lab pdf including all unapproved solutions */
function _latex_generate_files($lab_id, $full_lab = FALSE)
{

  $root_path = lab_migration_path();
  $dir_path = $root_path . "latex/";

  $lab_filedata = "";
  $solution_provider_filedata = "";
  $latex_filedata = "";
  $latex_dep_filedata = "";

  $depedency_list = array();

  $eol = "\n";
  $sep = "#";

  //$proposal_q = db_query("SELECT * FROM {lab_migration_proposal} WHERE id = %d", $lab_id);
  $query = db_select('lab_migration_proposal');
              $query->fields('lab_migration_proposal');
              $query->condition('id', $lab_id);
              $proposal_q = $query->execute();
  $proposal_data = $proposal_q->fetchObject();
  if (!$proposal_data)
  {
    drupal_set_message('Invalid lab specified.', 'error');
    drupal_goto('');
  }
  if ($proposal_data->approval_status == 0)
  {
    drupal_set_message('Lab proposal is still in pending review.', 'error');
    drupal_goto('');
  }

	$category_data = '';
	switch ($proposal_data->category)
	{
	case 0: $category_data = 'Not Selected'; break;
	case 1: $category_data = 'Fluid Mechanics'; break;
	case 2: $category_data = 'Control Theory & Control Systems'; break;
	case 3: $category_data = 'Chemical Engineering'; break;
	case 4: $category_data = 'Thermodynamics'; break;
	case 5: $category_data = 'Mechanical Engineering'; break;
	case 6: $category_data = 'Signal Processing'; break;
	case 7: $category_data = 'Digital Communications'; break;
	case 8: $category_data = 'Electrical Technology'; break;
	case 9: $category_data = 'Mathematics & Pure Science'; break;
	case 10: $category_data = 'Analog Electronics'; break;
	case 11: $category_data = 'Digital Electronics'; break;
	case 12: $category_data = 'Computer Programming'; break;
	case 13: $category_data = 'Others'; break;
	default: $category_data = 'Unknown'; break;
	}

  $lab_filedata = $proposal_data->lab_title . $sep . $proposal_data->name_title . $sep . $proposal_data->name . $sep . $proposal_data->department . $sep . $proposal_data->university . $sep . $category_data . $eol;

  /* regenerate lab if full lab selected */
  if ($full_lab){
  	lab_migration_del_lab_pdf($proposal_data->id);
}
  /* check if lab already generated */
  if (file_exists($dir_path . "lab_" . $proposal_data->id . ".pdf"))
  {
		/* download zip file */
		header('Content-Type: application/pdf');
		header('Content-disposition: attachment; filename="' . $proposal_data->lab_title . '.pdf"');
		header('Content-Length: ' . filesize($dir_path . "lab_" . $proposal_data->id . ".pdf"));
		readfile($dir_path . "lab_" . $proposal_data->id . ".pdf");
		return;
  }

  $solution_provider_user = user_load($proposal_data->solution_provider_uid);
  if (!$solution_provider_user)
  {
    drupal_set_message('Could not fetch solution provider information for the lab specified.', 'error');
  }
  $solution_provider_filedata .= $proposal_data->solution_provider_name_title . $sep . $proposal_data->solution_provider_name . $sep . $proposal_data->solution_provider_department . $sep . $proposal_data->solution_provider_university . $eol;

  //$experiment_q = db_query("SELECT * FROM {lab_migration_experiment} WHERE proposal_id = %d ORDER BY number DESC", $proposal_data->id);
  $query = db_select('lab_migration_experiment');
                $query->fields('lab_migration_experiment');
                $query->condition('proposal_id', $proposal_data->id);
                $query->orderBy('number', 'DESC');
                $experiment_q = $query->execute();
  while ($experiment_data = $experiment_q->fetchObject())
  {
  	if ($full_lab){
  		//$solution_q = db_query("SELECT * FROM {lab_migration_solution} WHERE experiment_id = %d ORDER BY code_number DESC", $experiment_data->id);
    $query = db_select('lab_migration_solution');
                $query->fields('lab_migration_solution');
                $query->condition('experiment_id', $experiment_data->id);
                $query->orderBy('code_number', 'DESC');
                $solution_q = $query->execute(); 
  	}else{
    	//$solution_q = db_query("SELECT * FROM {lab_migration_solution} WHERE experiment_id = %d AND approval_status = 1 ORDER BY code_number DESC", $experiment_data->id);
    $query = db_select('lab_migration_solution');
                $query->fields('lab_migration_solution');
                $query->condition('experiment_id', $experiment_data->id);
                $query->condition('approval_status', 1);
                $query->orderBy('code_number', 'DESC');
                $solution_q = $query->execute();
}
    while ($solution_data = $solution_q->fetchObject())
    {
      //$solution_files_q = db_query("SELECT * FROM {lab_migration_solution_files} WHERE solution_id = %d", $solution_data->id);
      $query = db_select('lab_migration_solution_files');
                    $query->fields('lab_migration_solution_files');
                    $query->condition('solution_id', $solution_data->id);
                    $solution_files_q = $query->execute();
      while ($solution_files_data = $solution_files_q->fetchObject())
      {
        $latex_filedata .= $experiment_data->number . $sep;
        $latex_filedata .= $experiment_data->title . $sep;
        $latex_filedata .= $solution_data->code_number . $sep;
        $latex_filedata .= $solution_data->caption . $sep;
        $latex_filedata .= $solution_files_data->filename . $sep;
        $latex_filedata .= $solution_files_data->filepath . $sep;
        $latex_filedata .= $solution_files_data->filetype . $sep;
        $latex_filedata .= $sep;
        $latex_filedata .= $solution_files_data->id;
        $latex_filedata .= $eol;
      }
      //$dependency_files_q = db_query("SELECT * FROM {lab_migration_solution_dependency} WHERE solution_id = %d", $solution_data->id);
      $query = db_select('lab_migration_solution_dependency');
                  $query->fields('lab_migration_solution_dependency');
                  $query->condition('solution_id', $solution_data->id);
                  $dependency_files_q = $query->execute();
      while ($dependency_files_data = $dependency_files_q->fetchObject())
      {
        //$dependency_q = db_query("SELECT * FROM {lab_migration_dependency_files} WHERE id = %d LIMIT 1", $dependency_files_data->dependency_id);
        $query = db_select('lab_migration_dependency_files');
                    $query->fields('lab_migration_dependency_files');
                    $query->condition('id', $dependency_files_data->dependency_id);
                    $query->range(0, 1);
                    $dependency_q = $query->execute();
        if ($dependency_data = $dependency_q->fetchObject())
        {
		if(substr($dependency_data->filename, -3) != "wav") {
		  $latex_filedata .= $experiment_data->number . $sep;
		  $latex_filedata .= $experiment_data->title . $sep;
		  $latex_filedata .= $solution_data->code_number . $sep;
		  $latex_filedata .= $solution_data->caption . $sep;
		  $latex_filedata .= $dependency_data->filename . $sep;
		  $latex_filedata .= $dependency_data->filepath . $sep;
		  $latex_filedata .= 'D' . $sep;
		  $latex_filedata .= $dependency_data->caption . $sep;
		  $latex_filedata .= $dependency_data->id;
		  $latex_filedata .= $eol;

		  $depedency_list[$dependency_data->id] = "D";
		}
 
        }
      }
    }
  }

  foreach ($depedency_list as $row => $data) {
    //$dependency_q = db_query("SELECT * FROM {lab_migration_dependency_files} WHERE id = %d LIMIT 1", $row);
    $query = db_select('lab_migration_dependency_files');
                $query->fields('lab_migration_dependency_files');
                $query->condition('id', $row);
                $query->range(0, 1);
                $dependency_q = $query->execute();
    if ($dependency_data = $dependency_q->fetchObject())
    {
      $latex_dep_filedata .= $dependency_data->filename . $sep;
      $latex_dep_filedata .= $dependency_data->filepath . $sep;
      $latex_dep_filedata .= $dependency_data->caption . $sep;
      $latex_dep_filedata .= $dependency_data->id;
      $latex_dep_filedata .= $eol;
    }
  }

  /**************************** WRITE TO FILES ********************************/
  $download_filename = $proposal_data->lab;
  $lab_filename = "tmp_" . $proposal_data->id . "_lab.txt";
  $solution_provider_filename = "tmp_" . $proposal_data->id . "_solution_provider.txt";
  $latex_filename = "tmp_" . $proposal_data->id . "_data.txt";
  $latex_dep_filename = "tmp_" . $proposal_data->id . "_dep_data.txt";
  $pdf_filename = "lab_" . $proposal_data->id . ".pdf";

  $fb = fopen($dir_path . $lab_filename, 'w');
  fwrite($fb, $lab_filedata);
  fclose($fb);

  $fc = fopen($dir_path . $solution_provider_filename, 'w');
  fwrite($fc, $solution_provider_filedata);
  fclose($fc);

  $fl = fopen($dir_path . $latex_filename, 'w');
  fwrite($fl, $latex_filedata);
  fclose($fl);

  $fd = fopen($dir_path . $latex_dep_filename, 'w');
  fwrite($fd, $latex_dep_filedata);
  fclose($fd); 
  if (_latex_run_script($lab_filename, $solution_provider_filename, $latex_filename, $latex_dep_filename, $pdf_filename))
  {
		/* download zip file */
		header('Content-Type: application/pdf');
		header('Content-disposition: attachment; filename="' . $proposal_data->lab_title . '_' . $proposal_data->name_title . '_' . $proposal_data->name . '.pdf"');
		header('Content-Length: ' . filesize($dir_path . $pdf_filename));
		readfile($dir_path . $pdf_filename);
	} else {
		drupal_set_message("Error occurred when generating the PDF version of the Book.", 'error');
	}

  /*********************** DELETING TEMPORARY FILES ***************************/
  /* regenerate lab if full lab selected */
  if ($full_lab)
  	lab_migration_del_lab_pdf($proposal_data->id);
}

function _latex_copy_script_file()
{
  exec("cp ./" . drupal_get_path('module', 'lab_migration') . "/latex/* " . lab_migration_path() . "latex");
  exec("chmod u+x ./uploads/latex/*.sh");
}

function _latex_run_script($lab_filename, $solution_provider_filename, $latex_filename, $latex_dep_filename, $pdf_filename)
{
  $root_path = lab_migration_path();
  $ret = 0;
  chdir("lab_migration_uploads");
  chdir("latex");

  $sh_command = "./pdf_creator.sh " . $lab_filename . " " . $solution_provider_filename . " " . $latex_filename . " " . $latex_dep_filename;
  exec($sh_command);
  exec("mv TEX_final.pdf " . $pdf_filename);

  if ($ret == 0)
    return TRUE;
  else
    return FALSE;
}

function lab_migration_delete_lab_pdf()
{
	$lab_id = arg(3);
	lab_migration_del_lab_pdf($lab_id);
	drupal_set_message(t('Lab schedule for regeneration.'), 'status');
	drupal_goto('lab_migration/code_approval/bulk');
	return;
}

