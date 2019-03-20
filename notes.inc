<?php
// $Id$

/******************************************************************************/
/***************************** BOOK NOTES *************************************/
/******************************************************************************/

function lab_migration_lab_notes_form($form_state)
{
  global $user;

  /* get current proposal */
  $proposal_id = (int)arg(3);
  //$proposal_q = db_query("SELECT * FROM {lab_migration_proposal} WHERE id = %d LIMIT 1", $proposal_id);
  $query = db_select('lab_migration_proposal');
              $query->fields('lab_migration_proposal');
              $query->condition('id', $proposal_id);
              $query->range(0, 1);
              $proposal_q = $query->execute();
  $proposal_data = $proposal_q->fetchObject();
  if (!$proposal_data)
  {
    drupal_set_message(t('Invalid lab selected. Please try again.'), 'error');
    drupal_goto('lab_migration/code_approval');
    return;
  }

  /* get current notes */
  $notes = '';
  //$notes_q = db_query("SELECT * FROM {lab_migration_notes} WHERE proposal_id = %d LIMIT 1", $proposal_id);
  $query = db_select('lab_migration_notes');
              $query->fields('lab_migration_notes');
              $query->condition('proposal_id', $proposal_id);
              $query->range(0, 1);
              $notes_q = $query->execute();

  if ($notes_q)
  {
    $notes_data = $notes_q->fetchObject();
    $notes = $notes_data->notes;
  }

  $form['lab_details'] = array(
    '#type' => 'item',
    '#value' => '<span style="color: rgb(128, 0, 0);"><strong>About the Lab</strong></span><br />' .
      '<strong>Proposer:</strong> ' . $proposal_data->name . '<br />' .
      '<strong>Title of the Lab:</strong> ' . $proposal_data->lab_title . '<br />'
  );

  $form['notes'] = array(
    '#type' => 'textarea',
    '#rows' => 20,
    '#title' => t('Notes for Reviewers'),
    '#default_value' => $notes,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit')
  );

  $form['cancel'] = array(
    '#type' => 'markup',
    '#value' => l(t('Back'), 'lab_migration/code_approval'),
  );
  return $form;
}

function lab_migration_lab_notes_form_submit($form, &$form_state)
{
  global $user;

  /* get current proposal */
  $proposal_id = (int)arg(3);
  //$proposal_q = db_query("SELECT * FROM {lab_migration_proposal} WHERE id = %d LIMIT 1", $proposal_id);
  $query = db_select('lab_migration_proposal');
              $query->fields('lab_migration_proposal');
              $query->condition('id', $proposal_id);
              $query->range(0, 1);
              $proposal_q = $query->execute();
  $proposal_data = $proposal_q->fetchObject();
  if (!$proposal_data)
  {
    drupal_set_message(t('Invalid lab selected. Please try again.'), 'error');
    drupal_goto('lab_migration/code_approval');
    return;
  }

  /* find existing notes */
  //$notes_q = db_query("SELECT * FROM {lab_migration_notes} WHERE proposal_id = %d LIMIT 1", $proposal_id);
  $query = db_select('lab_migration_notes');
                $query->fields('lab_migration_notes');
                $query->condition('proposal_id', $proposal_id);
                $query->range(0, 1);
                $notes_q = $query->execute();


  $notes_data = $notes_q->fetchObject();

  /* add or update notes in database */
  if ($notes_data) {    
    $query = "UPDATE {lab_migration_notes} SET notes = :notes WHERE id = :notes_id";
    $args =  array(
                ":notes" => $form_state['values']['notes'], 
                ":notes_id" => $notes_data->id,
              );
    db_query($query, $args);
    drupal_set_message('Notes updated successfully.', 'status');
  } else {
    $query = "INSERT INTO {lab_migration_notes} (proposal_id, notes) VALUES (:proposal_id, :notes)";
    $args =  array(
                ":proposal_id" => $proposal_id, 
                ":notes" =>$form_state['values']['notes'],
              );
    db_query($query, $args);
    drupal_set_message('Notes added successfully.', 'status');
  }
}

