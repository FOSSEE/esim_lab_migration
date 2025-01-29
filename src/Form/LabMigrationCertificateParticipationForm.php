<?php

/**
 * @file
 * Contains \Drupal\lab_migration\Form\LabMigrationCertificateParticipationForm.
 */

namespace Drupal\lab_migration\Form;

use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Render\Element;

class LabMigrationCertificateParticipationForm extends FormBase {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'lab_migration_certificate_participation_form';
  }

  public function buildForm(array $form, \Drupal\Core\Form\FormStateInterface $form_state) {
    $form['name_title'] = [
      '#type' => 'select',
      '#title' => t('Title'),
      '#options' => [
        'Dr.' => 'Dr.',
        'Prof.' => 'Prof.',
        'Mr.' => 'Mr.',
        'Mrs.' => 'Mrs.',
        'Ms.' => 'Ms.',
      ],
      '#required' => TRUE,
    ];
    $form['name'] = [
      '#type' => 'textfield',
      '#title' => t('Name of Participant'),
      '#maxlength' => 50,
      '#required' => TRUE,
    ];
    $form['email_id'] = [
      '#type' => 'textfield',
      '#title' => t('Email'),
      '#size' => 50,
      '#default_value' => 'Not availbale',
    ];
    $form['institute_name'] = [
      '#type' => 'textfield',
      '#title' => t('Collage / Institue Name'),
      '#required' => TRUE,
    ];
    $form['institute_address'] = [
      '#type' => 'textfield',
      '#title' => t('Collage / Institue address'),
      '#required' => TRUE,
    ];
    $form['lab_name'] = [
      '#type' => 'textfield',
      '#title' => t('Lab name'),
      '#required' => TRUE,
    ];
    $form['department'] = [
      '#type' => 'textfield',
      '#title' => t('Department'),
      '#required' => TRUE,
    ];
    $form['proposal_id'] = [
      '#type' => 'textfield',
      '#title' => t('Lab Proposal Id'),
      '#description' => 'Note: You can find  the respective Lab Proposal Id from the url for the  completed lab. For example: The Lab Proposal Id is 64 for this completed lab. ( Url - scilab.in/lab_migration_run/64)',
      '#required' => TRUE,
    ];
    $form['submit'] = [
      '#type' => 'submit',
      '#value' => t('Submit'),
    ];
    return $form;
  }

  public function submitForm(array &$form, \Drupal\Core\Form\FormStateInterface $form_state) {
    $user = \Drupal::currentUser();
    $v = $form_state->getValues();
    $result = "INSERT INTO {lab_migration_certificate} 
    (uid, name_title, name, email_id, institute_name, institute_address, lab_name, department, proposal_id,creation_date) VALUES
    (:uid, :name_title, :name, :email_id, :institute_name, :institute_address, :lab_name, :department, :proposal_id,:creation_date)";
    $args = [
      ":uid" => $user->uid,
      ":name_title" => trim($v['name_title']),
      ":name" => trim($v['name']),
      ":email_id" => trim($v['email_id']),
      ":institute_name" => trim($v['institute_name']),
      ":institute_address" => trim($v['institute_address']),
      ":lab_name" => trim($v['lab_name']),
      ":department" => trim($v['department']),
      ":proposal_id" => $v['proposal_id'],
      ":creation_date" => time(),
    ];
    $proposal_id = \Drupal::database()->query($result, $args);
    drupal_goto('lab_migration/certificate');
  }

}
?>
