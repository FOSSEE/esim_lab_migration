<?php
// $Id$

function lab_migration_bulk_upload_code_form($form_state)
{
  global $user;

  $proposal_id = (int)arg(3);
  //$proposal_q = db_query("SELECT * FROM {lab_migration_proposal} WHERE id = %d", $proposal_id);
  $query = db_select('lab_migration_proposal');
            $query->fields('lab_migration_proposal');
            $query->condition('id', $proposal_id);
            $proposal_q = $query->execute();
  $proposal_data = $proposal_q->fetchObject();
  if (!$proposal_data)
  {
    drupal_set_message("Invalid proposal selected", 'error');
    drupal_goto('lab_migration/code_approval/bulk');
  }

  /* add javascript for dependency selection effects */
  $dep_selection_js = "(function ($) {
    $('#edit-existing-depfile-dep-lab-title').change(function() {
      var dep_selected = ''; 
      /* showing and hiding relevant files */
      $('.form-checkboxes .option').hide();
      $('.form-checkboxes .option').each(function(index) {
        var activeClass = $('#edit-existing-depfile-dep-lab-title').val();
        if ($(this).children().hasClass(activeClass)) {
          $(this).show();
        }
        if ($(this).children().attr('checked') == true) {
          dep_selected += $(this).children().next().text() + '<br />';
        }
      });
      /* showing list of already existing dependencies */
      $('#existing_depfile_selected').html(dep_selected);
    });

    $('.form-checkboxes .option').change(function() {
      $('#edit-existing-depfile-dep-lab-title').trigger('change');
    });
    $('#edit-existing-depfile-dep-lab-title').trigger('change');
  })(jQuery);";
  drupal_add_js($dep_selection_js, 'inline', 'header');

  $form['#attributes'] = array('enctype' => "multipart/form-data");

  $form['lab_title'] = array(
    '#type' => 'item',
    '#value' => $proposal_data->lab_title,
    '#title' => t('Title of the Lab'),
  );
  $form['name'] = array(
    '#type' => 'item',
    '#value' => $proposal_data->name_title . ' ' . $proposal_data->name,
    '#title' => t('Proposer Name'),
  );

  /* get experiment list */
  $experiment_rows = array();
  //$experiment_q = db_query("SELECT * FROM {lab_migration_experiment} WHERE proposal_id = %d ORDER BY id ASC", $proposal_data->id);
  $query = db_select('lab_migration_experiment');
            $query->fields('lab_migration_experiment');
            $query->condition('proposal_id', $proposal_data->id);
            $query->orderBy('id', 'ASC');
            $experiment_q = $query->execute();
  while ($experiment_data = $experiment_q->fetchObject())
  {
    $experiment_rows[$experiment_data->id] = $experiment_data->number . '. ' . $experiment_data->title;
  }
  $form['experiment'] = array(
    '#type' => 'select',
    '#title' => t('Title of the Experiment'),
    '#options' => $experiment_rows,
    '#multiple' => FALSE,
    '#size' => 1,
    '#required' => TRUE,
  );

  $form['code_number'] = array(
    '#type' => 'textfield',
    '#title' => t('Code No'),
    '#size' => 5,
    '#maxlength' => 10,
    '#description' => t(""),
    '#required' => TRUE,
  );
  $form['code_caption'] = array(
    '#type' => 'textfield',
    '#title' => t('Caption'),
    '#size' => 40,
    '#maxlength' => 255,
    '#description' => t(''),
    '#required' => TRUE,
  );
  $form['code_warning'] = array(
    '#type' => 'item',
    '#title' => t('Upload all the eSim project files in .zip format'),
    '#prefix' => '<div style="color:red">',
    '#suffix' => '</div>',
  );

  $form['sourcefile'] = array(
    '#type' => 'fieldset',
    '#title' => t('Main or Source Files'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );
  $form['sourcefile']['sourcefile1'] = array(
    '#type' => 'file',
    '#title' => t('Upload main or source file'),
    '#size' => 48,
    '#description' => t('Separate filenames with underscore. No spaces or any special characters allowed in filename.') . '<br />' .
    t('Allowed file extensions : ') . variable_get('lab_migration_source_extensions', ''),
  );

  $form['dep_files'] = array(
    '#type' => 'item',
    '#title' => t('Dependency Files'),
  );

  /************ START OF EXISTING DEPENDENCIES **************/

  /* existing dependencies */
  $form['existing_depfile'] = array(
    '#type' => 'fieldset',
    '#title' => t('Use Already Existing Dependency Files'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
    '#prefix' => '<div id="existing-depfile-wrapper">',
    '#suffix' => '</div>',
    '#tree' => TRUE,
  );

  /* existing dependencies */
  $form['existing_depfile']['selected'] = array(
    '#type' => 'item',
    '#title' => t('Existing Dependency Files Selected'),
    '#value' => '<div id="existing_depfile_selected"></div>',
  );

  $form['existing_depfile']['dep_lab_title'] = array(
    '#type' => 'select',
    '#title' => t('Title of the Lab'),
    '#options' => _list_of_lab_titles(),
  );

  list($files_options, $files_options_class) = _list_of_dependency_files();
  $form['existing_depfile']['dep_experiment_files'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Dependency Files'),
    '#options' => $files_options,
    '#options_class' => $files_options_class,
    '#multiple' => TRUE,
  );

  $form['existing_depfile']['dep_upload'] = array(
    '#type' => 'item',
    '#value' => l('Upload New Depedency Files', 'lab_migration/code/upload_dep'),
  );
  /************ END OF EXISTING DEPENDENCIES **************/

  $form['result'] = array(
    '#type' => 'fieldset',
    '#title' => t('Result Files'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );
  $form['result']['result1'] = array(
    '#type' => 'file',
    '#title' => t('Upload result file'),
    '#size' => 48,
    '#description' => t('Separate filenames with underscore. No spaces or any special characters allowed in filename.') . '<br />' .
    t('Allowed file extensions : ') . variable_get('lab_migration_result_extensions', ''),
  );
  $form['result']['result2'] = array(
    '#type' => 'file',
    '#title' => t('Upload result file'),
    '#size' => 48,
    '#description' => t('Separate filenames with underscore. No spaces or any special characters allowed in filename.') . '<br />' .
    t('Allowed file extensions : ') . variable_get('lab_migration_result_extensions', ''),
  );

  $form['xcos'] = array(
    '#type' => 'fieldset',
    '#title' => t('XCOS Files'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );
  $form['xcos']['xcos1'] = array(
    '#type' => 'file',
    '#title' => t('Upload xcos file'),
    '#size' => 48,
    '#description' => t('Separate filenames with underscore. No spaces or any special characters allowed in filename.') . '<br />' .
    t('Allowed file extensions : ') . variable_get('lab_migration_xcos_extensions', ''),
  );
  $form['xcos']['xcos2'] = array(
    '#type' => 'file',
    '#title' => t('Upload xcos file'),
    '#size' => 48,
    '#description' => t('Separate filenames with underscore. No spaces or any special characters allowed in filename.') . '<br />' .
    t('Allowed file extensions : ') . variable_get('lab_migration_xcos_extensions', ''),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit')
  );

  $form['cancel'] = array(
    '#type' => 'markup',
    '#value' => l(t('Cancel'), 'lab_migration/code_approval/bulk'),
  );
  return $form;
}

function lab_migration_bulk_upload_code_form_validate($form, &$form_state)
{
  if (!lab_migration_check_code_number($form_state['values']['code_number']))
    form_set_error('code_number', t('Invalid Code Number. Code Number can contain only numbers.'));

  if (!lab_migration_check_name($form_state['values']['code_caption']))
    form_set_error('code_caption', t('Caption can contain only alphabets, numbers and spaces.'));

  if (isset($_FILES['files']))
  {
    /* check if atleast one source or result file is uploaded */  
    if ( ! ($_FILES['files']['name']['sourcefile1'] || $_FILES['files']['name']['xcos1']))
      form_set_error('sourcefile1', t('Please upload atleast one main or source file or xcos file.'));

    /* check for valid filename extensions */  
    foreach ($_FILES['files']['name'] as $file_form_name => $file_name)
    {
      if ($file_name)
      {
        /* checking file type */
        if (strstr($file_form_name, 'source'))
          $file_type = 'S';
        else if (strstr($file_form_name, 'result'))
          $file_type = 'R';
        else if (strstr($file_form_name, 'xcos'))
          $file_type = 'X';
        else
          $file_type = 'U';

        $allowed_extensions_str = '';
        switch ($file_type)
        {
        case 'S':
          $allowed_extensions_str = variable_get('lab_migration_source_extensions', '');
          break;
        case 'R':
          $allowed_extensions_str = variable_get('lab_migration_result_extensions', '');
          break;
        case 'X':
          $allowed_extensions_str = variable_get('lab_migration_xcos_extensions', '');
          break;
        }

        $allowed_extensions = explode(',' , $allowed_extensions_str);
        $temp_extension = end(explode('.', strtolower($_FILES['files']['name'][$file_form_name])));
        if (!in_array($temp_extension, $allowed_extensions))
          form_set_error($file_form_name, t('Only file with ' . $allowed_extensions_str . ' extensions can be uploaded.'));
        if ($_FILES['files']['size'][$file_form_name] <= 0)
          form_set_error($file_form_name, t('File size cannot be zero.'));

        /* check if valid file name */
        if (!lab_migration_check_valid_filename($_FILES['files']['name'][$file_form_name]))
         form_set_error($file_form_name, t('Invalid file name specified. Only alphabets and numbers are allowed as a valid filename.'));
      }
    }
  }

  /* add javascript dependency selection effects */
  $dep_selection_js =  "(function ($) {
    $('#edit-existing-depfile-dep-lab-title').change(function() {
      var dep_selected = ''; 
      /* showing and hiding relevant files */
      $('.form-checkboxes .option').hide();
      $('.form-checkboxes .option').each(function(index) {
        var activeClass = $('#edit-existing-depfile-dep-lab-title').val();
        if ($(this).children().hasClass(activeClass)) {
          $(this).show();
        }
        if ($(this).children().attr('checked') == true) {
          dep_selected += $(this).children().next().text() + '<br />';
        }
      });
      /* showing list of already existing dependencies */
      $('#existing_depfile_selected').html(dep_selected);
    });

    $('.form-checkboxes .option').change(function() {
      $('#edit-existing-depfile-dep-lab-title').trigger('change');
    });
    $('#edit-existing-depfile-dep-lab-title').trigger('change');
  }(jQuery));";
  drupal_add_js($dep_selection_js, 'inline', 'header');
}

function lab_migration_bulk_upload_code_form_submit($form, &$form_state) {
  global $user;

  $root_path = lab_migration_path();

  $proposal_id = (int)arg(3);
  //$proposal_q = db_query("SELECT * FROM {lab_migration_proposal} WHERE id = %d", $proposal_id);
  $query = db_select('lab_migration_proposal');
            $query->fields('lab_migration_proposal');
            $query->condition('id', $proposal_id);
            $proposal_q = $query->execute();
  $proposal_data = $proposal_q->fetchObject();
  if (!$proposal_data)
  {
    drupal_set_message("Invalid proposal selected", 'error');
    drupal_goto('lab_migration/code_approval/upload/' . $proposal_id);
  }

  $proposal_id = $proposal_data->id;

  /************************ check experiment details ************************/
  $experiment_id = (int)$form_state['values']['experiment'];
  //$experiment_q = db_query("SELECT * FROM {lab_migration_experiment} WHERE id = %d AND proposal_id = %d LIMIT 1", $experiment_id, $proposal_id);
  $query = db_select('lab_migration_experiment');
            $query->fields('lab_migration_experiment');
            $query->condition('id', $experiment_id);
            $query->condition('proposal_id', $proposal_id);
            $query->range(0, 1);
            $experiment_q = $query->execute();
  $experiment_data = $experiment_q->fetchObject();
  if (!$experiment_data)
  {
    drupal_set_message("Invalid experiment seleted", 'error');
    drupal_goto('lab_migration/code_approval/upload/' . $proposal_id);
  }

  /* create proposal folder if not present */
  $dest_path = $proposal_id . '/';
  if (!is_dir($root_path . $dest_path))
    mkdir($root_path . $dest_path);

  /*  get solution details - dont allow if already solution present */
 // $cur_solution_q = db_query("SELECT * FROM {lab_migration_solution} WHERE experiment_id = %d AND code_number = '%s'", $experiment_id, $experiment_data->number . '.' . $form_state['values']['code_number']);
  $code_number = $experiment_data->number . '.' . $form_state['values']['code_number']; 
  $query = db_select('lab_migration_solution');
            $query->fields('lab_migration_solution');
            $query->condition('experiment_id', $experiment_id);
            $query->condition('code_number', $code_number);
$cur_solution_q = $query->execute();
  if ($cur_solution_d = $cur_solution_q->fetchObject())
  {
    if ($cur_solution_d->approval_status == 1)
    {
      drupal_set_message(t("Solution already approved. Cannot overwrite it."), 'error');
      drupal_goto('lab_migration/code_approval/upload/' . $proposal_id);
      return;
    } else if ($cur_solution_d->approval_status == 0) {
      drupal_set_message(t("Solution is under pending review. Delete the solution and reupload it."), 'error');
      drupal_goto('lab_migration/code_approval/upload/' . $proposal_id);
      return;
    } else {
      drupal_set_message(t("Error uploading solution. Please contact administrator."), 'error');
      drupal_goto('lab_migration/code_approval/upload/' . $proposal_id);
      return;
    }
  }

  /* creating experiment directories */
  $dest_path .= 'EXP' . $experiment_data->number . '/';
  if (!is_dir($root_path . $dest_path))
    mkdir($root_path . $dest_path);

  /* creating code directories */
  $dest_path .= 'CODE' . $experiment_data->number . '.' . $form_state['values']['code_number'] . '/';
  if (!is_dir($root_path . $dest_path))
    mkdir($root_path . $dest_path);

  /* creating solution database entry */
 
   $query ="INSERT INTO {lab_migration_solution} (experiment_id, approver_uid, code_number, caption, approval_date, approval_status, timestamp) VALUES (:experiment_id, :approver_uid, :code_number, :caption, :approval_date, :approval_status, :timestamp)";
   $args = array(
    ":experiment_id" => $experiment_id,
    ":approver_uid" => 0,   
    ":code_number" => $experiment_data->number . '.' . $form_state['values']['code_number'],
    ":caption" => $form_state['values']['code_caption'],
    ":approval_date" => 0,
    ":approval_status" => 0,
    ":timestamp" => time()
  );
  
  $solution_id = db_query($query, $args, array('return' => Database::RETURN_INSERT_ID));

  /* linking existing dependencies */
  foreach ($form_state['values']['existing_depfile']['dep_experiment_files'] as $row)
  {
    if ($row > 0)
    {
      /* insterting into database */ 
      $query = "INSERT INTO {lab_migration_solution_dependency} (solution_id, dependency_id)
        VALUES (:solution_id, :dependency_id)";
        $args = array(
           ":solution_id" => $solution_id,
           ":dependency_id" => $row
          );
        db_query($query, $args);
    }
  }

  /* uploading files */
  foreach ($_FILES['files']['name'] as $file_form_name => $file_name)
  {
    if ($file_name)
    {
      /* checking file type */
      if (strstr($file_form_name, 'source'))
        $file_type = 'S';
      else if (strstr($file_form_name, 'result'))
        $file_type = 'R';
      else if (strstr($file_form_name, 'xcos'))
        $file_type = 'X';
      else
        $file_type = 'U';

      if (file_exists($root_path . $dest_path . $_FILES['files']['name'][$file_form_name]))
      {
        drupal_set_message(t("Error uploading file. File !filename already exists.", array('!filename' => $_FILES['files']['name'][$file_form_name])), 'error');
        return;
      }

      /* uploading file */
      if (move_uploaded_file($_FILES['files']['tmp_name'][$file_form_name], $root_path . $dest_path . $_FILES['files']['name'][$file_form_name]))
      {
        /* for uploaded files making an entry in the database */      
        $query = "INSERT INTO {lab_migration_solution_files} (solution_id, filename, filepath, filemime, filesize, filetype, timestamp)
          VALUES (:solution_id, :filename, :filepath, :filemime, :filesize, :filetype, :timestamp)";
          $args = array (
            ":solution_id" => $solution_id,
            ":filename" => $_FILES['files']['name'][$file_form_name],
            ":filepath" => $dest_path . $_FILES['files']['name'][$file_form_name], 
            ":filemime" => $_FILES['files']['type'][$file_form_name],
            ":filesize" => $_FILES['files']['size'][$file_form_name],
            ":filetype" => $file_type,
            ":timestamp" => time()
            );
          db_query($query, $args);
        drupal_set_message($file_name . ' uploaded successfully.', 'status');
      } else {
        drupal_set_message('Error uploading file : ' . $dest_path . '/' . $file_name, 'error');
      }
    }
  }
  drupal_set_message('Solution uploaded successfully.', 'status');

  /* sending email */

  $email_to = $user->mail;

  $from = variable_get('lab_migration_from_email', '');
  $bcc= variable_get('lab_migration_emails', '');
  $cc=variable_get('lab_migration_cc_emails', '');
  
  $param['solution_uploaded']['solution_id'] = $solution_id;
  $param['solution_uploaded']['user_id'] = $user->uid;
  $param['solution_uploaded']['headers']=array('From'=>$from,'MIME-Version'=> '1.0',
    			'Content-Type'=> 'text/plain; charset=UTF-8; format=flowed; delsp=yes',
    			'Content-Transfer-Encoding' => '8Bit',
    			'X-Mailer'=> 'Drupal','Cc' => $cc, 'Bcc' => $bcc);

  if (!drupal_mail('lab_migration', 'solution_uploaded', $email_to, language_default(), $param, $from, TRUE))
    drupal_set_message('Error sending email message.', 'error');

  drupal_goto('lab_migration/code_approval/bulk/');
}

/******************************************************************************/
/************************** GENERAL FUNCTIONS *********************************/
/******************************************************************************/

function _list_of_lab_titles()
{
  $lab_titles = array('0' => 'Please select...');
  //$lab_titles_q = db_query("SELECT * FROM {lab_migration_proposal} WHERE approval_status = 1 OR approval_status = 3 ORDER BY lab_title ASC");
  $query = db_select('lab_migration_proposal');
  $query->fields('lab_migration_proposal');

  $or = db_or();
  $or->condition('approval_status', 1);
  $or->condition('approval_status', 3);
  $query->condition($or);
  $query->orderBy('lab_title', 'ASC');
  $lab_titles_q = $query->execute(); 

  while ($lab_titles_data = $lab_titles_q->fetchObject())
  {
    $lab_titles[$lab_titles_data->id] = $lab_titles_data->lab_title . ' (Proposed by ' . $lab_titles_data->name . ')';
  }
  return $lab_titles;
}

function _list_of_dependency_files()
{
  $dependency_files = array();
  $dependency_files_class = array();
  //$dependency_files_q = db_query("SELECT * FROM {lab_migration_dependency_files} ORDER BY filename ASC");
  $query = db_select('lab_migration_dependency_files');
            $query->fields('lab_migration_dependency_files');
            $query->orderBy('filename', 'ASC');
            $dependency_files_q = $query->execute();

  while ($dependency_files_data = $dependency_files_q->fetchObject())
  {
    $temp_caption = '';
    if ($dependency_files_data->caption)
      $temp_caption .= ' (' . $dependency_files_data->caption . ')';
    $dependency_files[$dependency_files_data->id] = l($dependency_files_data->filename . $temp_caption, 'lab_migration/download/dependency/' . $dependency_files_data->id);
    $dependency_files_class[$dependency_files_data->id] = $dependency_files_data->proposal_id;
  }
  return array($dependency_files, $dependency_files_class);
}

